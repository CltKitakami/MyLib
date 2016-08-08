#ifdef _WIN32

#if 0
#define PACKAGE 1
#define PACKAGE_VERSION 1

#include <string>
#include <map>
#include <iostream>
#include <iomanip>

#include <windows.h>
#include <excpt.h>
#include <imagehlp.h>
#include <psapi.h>
#include <bfd.h>

#include "pattern/Singleton.hpp"


#ifdef PLATFORM_X64
typedef DWORD64 address;
#else
typedef DWORD address;
#endif


static LONG WINAPI exceptionHandler(LPEXCEPTION_POINTERS info);


typedef struct FindInfo
{
	FindInfo() :
		symbol(nullptr), counter(0), file(nullptr),
		func(nullptr), line(0) {}
	asymbol **symbol;
	bfd_vma counter;
	const char *file;
	const char *func;
	unsigned int line;
} FindInfo;


class BfdContext
{
public:

	BfdContext(const char *porgramPath = nullptr) :
		handle(nullptr), symbol(nullptr), error(0), isInitialized(false)
	{
		if (porgramPath != nullptr)
			this->initialize(porgramPath);
	}

	~BfdContext()
	{
		if (this->handle != nullptr)
			bfd_close(this->handle);
		if (this->symbol != nullptr)
			::free(this->symbol);
	}

	void initialize(const char *porgramPath)
	{
		if (this->error == 0) this->open(porgramPath);
		if (this->error == 0) this->checkFormat();
		if (this->error == 0) this->readSymbols();
		if (this->error != 0)
			std::cerr << " from " << porgramPath << std::endl;
		else
			this->isInitialized = true;
	}

	void open(const char *porgramPath)
	{
		bfd *b = bfd_openr(porgramPath, 0);
		if (b == nullptr)
		{
			std::cerr << "Failed to open bfd";
			this->error = 1;
		}
		else
			this->handle = b;
	}

	void checkFormat()
	{
		if (bfd_check_format(this->handle, bfd_object) == 0 ||
			bfd_check_format_matches(this->handle, bfd_object, nullptr) == 0 ||
			(bfd_get_file_flags(this->handle) & HAS_SYMS) == 0)
		{
			std::cerr << "Failed to format bfd";
			this->error = 2;
		}
	}

	void readSymbols()
	{
		void **symbolTable = reinterpret_cast<void **>(&this->symbol);
		unsigned dummy = 0;
		if (bfd_read_minisymbols(this->handle, false, symbolTable, &dummy) == 0)
		{
			if (bfd_read_minisymbols(this->handle, true, symbolTable, &dummy) < 0)
			{
				std::cerr << "Failed to read symbols";
				this->error = 3;
				return;
			}
		}
	}

	bfd *handle;
	asymbol **symbol;
	int error;
	bool isInitialized;
};


class Backtrace
{
	typedef std::map<std::string, BfdContext> BfdSet;

	BfdSet set;
	static const int maxStackDepth;
	static LPTOP_LEVEL_EXCEPTION_FILTER oldExceptionHandler;

public:
	Backtrace()
	{
		Backtrace::oldExceptionHandler = ::SetUnhandledExceptionFilter(exceptionHandler);
	}

	~Backtrace()
	{
		if (Backtrace::oldExceptionHandler != nullptr)
		{
			::SetUnhandledExceptionFilter(Backtrace::oldExceptionHandler);
			Backtrace::oldExceptionHandler = nullptr;
		}
	}

	static void lookupSection(bfd *bfdPtr, asection *sectionPtr, void *outInfoPtr)
	{
		FindInfo *info = reinterpret_cast<FindInfo *>(outInfoPtr);

		if (info->func != nullptr ||
			(bfd_get_section_flags(bfdPtr, sectionPtr) & SEC_ALLOC) == 0)
			return;

		bfd_vma vma = bfd_get_section_vma(bfdPtr, sectionPtr);

		if (info->counter < vma ||
			vma + bfd_get_section_size(sectionPtr) <= info->counter)
			return;

		bfd_find_nearest_line(bfdPtr, sectionPtr,
			info->symbol, info->counter - vma,
			&info->file, &info->func, &info->line);
	}

	static FindInfo find(BfdContext &context, DWORD offset)
	{
		FindInfo info;
		info.symbol = context.symbol;
		info.counter = offset;
		bfd_map_over_sections(context.handle, &lookupSection, &info);
		return info;
	}

	static STACKFRAME initializeStackFrame(const LPCONTEXT &context)
	{
        STACKFRAME frame;
        memset(&frame, 0, sizeof(frame));
#ifdef PLATFORM_X64
        frame.AddrPC.Offset = context->Rip;
        frame.AddrStack.Offset = context->Rsp;
        frame.AddrFrame.Offset = context->Rbp;
#else
        frame.AddrPC.Offset = context->Eip;
        frame.AddrStack.Offset = context->Esp;
        frame.AddrFrame.Offset = context->Ebp;
#endif
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrStack.Mode = AddrModeFlat;
        frame.AddrFrame.Mode = AddrModeFlat;

		return frame;
	}

	static bool stackWalk(HANDLE process, HANDLE thread, STACKFRAME &frame, LPCONTEXT context)
	{
#ifdef PLATFORM_X64
		return ::StackWalk(IMAGE_FILE_MACHINE_AMD64,
#else
		return ::StackWalk(IMAGE_FILE_MACHINE_I386,
#endif
			process, thread, &frame, context, 0,
			SymFunctionTableAccess, SymGetModuleBase, 0) != 0;
	}

	static bool getModuleName(HINSTANCE moduleBase, std::string &outBuffer, int bufferSize = MAX_PATH)
	{
		int length = 0;
		if (moduleBase != 0)
		{
			outBuffer.resize(bufferSize);
			length = ::GetModuleFileNameA(moduleBase, &outBuffer[0], bufferSize);

			if (length != 0)
				outBuffer.resize(length);
		}

		if (length == 0)
		{
			outBuffer = "[unknown module]";
			return false;
		}
		else
			return true;
	}

	static void printInfo(int depth, const std::string &moduleName, const FindInfo &info)
	{
		std::cout << '#' << std::setw(2) << depth <<
			" 0x" << std::hex << std::setw(8) << info.counter << std::dec <<
			": " << moduleName << ": " << info.file;

		if (info.func != nullptr && info.func != nullptr)
			std::cout << ": " << info.line << ": " << info.func;

		std::cout << std::endl;
	}

	void report(LPCONTEXT context)
	{
        HANDLE process = ::GetCurrentProcess();
        HANDLE thread = ::GetCurrentThread();
		STACKFRAME frame = Backtrace::initializeStackFrame(context);

		std::string moduleName;
        char symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + 256];
		IMAGEHLP_SYMBOL *symbol = (IMAGEHLP_SYMBOL *)symbolBuffer;
		symbol->SizeOfStruct = (sizeof(*symbol)) + 256;
		symbol->MaxNameLength = 255;

		int depth = 0;
        while (++depth < maxStackDepth &&
			stackWalk(process, thread, frame, context) == true)
		{
			address moduleBase = ::SymGetModuleBase(process, frame.AddrPC.Offset);

			Backtrace::getModuleName((HINSTANCE)moduleBase, moduleName);
			if (set[moduleName].isInitialized == false)
				set[moduleName].initialize(moduleName.data());

			FindInfo info = Backtrace::find(set[moduleName], frame.AddrPC.Offset);

			if (info.file == nullptr || info.file == nullptr)
			{
				address dummy = 0;
				if (::SymGetSymFromAddr(process, frame.AddrPC.Offset, &dummy, symbol))
					info.file = symbol->Name;
				else
					info.file = "[unknown file]";
			}

			Backtrace::printInfo(depth, moduleName, info);
        }
	}
};


const int Backtrace::maxStackDepth = 100;
LPTOP_LEVEL_EXCEPTION_FILTER Backtrace::oldExceptionHandler = nullptr;


static void printExceptionCode(LPEXCEPTION_POINTERS info)
{
#define CASE(code) \
	case code:\
		std::cout << "code " << code << " (" #code "), "; break
	switch(info->ExceptionRecord->ExceptionCode)
	{
		CASE(EXCEPTION_ACCESS_VIOLATION);
		CASE(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
		CASE(EXCEPTION_BREAKPOINT);
		CASE(EXCEPTION_DATATYPE_MISALIGNMENT);
		CASE(EXCEPTION_FLT_DENORMAL_OPERAND);
		CASE(EXCEPTION_FLT_DIVIDE_BY_ZERO);
		CASE(EXCEPTION_FLT_INEXACT_RESULT);
		CASE(EXCEPTION_FLT_INVALID_OPERATION);
		CASE(EXCEPTION_FLT_OVERFLOW);
		CASE(EXCEPTION_FLT_STACK_CHECK);
		CASE(EXCEPTION_FLT_UNDERFLOW);
		CASE(EXCEPTION_ILLEGAL_INSTRUCTION);
		CASE(EXCEPTION_IN_PAGE_ERROR);
		CASE(EXCEPTION_INT_DIVIDE_BY_ZERO);
		CASE(EXCEPTION_INT_OVERFLOW);
		CASE(EXCEPTION_INVALID_DISPOSITION);
		CASE(EXCEPTION_NONCONTINUABLE_EXCEPTION);
		CASE(EXCEPTION_PRIV_INSTRUCTION);
		CASE(EXCEPTION_SINGLE_STEP);
		CASE(EXCEPTION_STACK_OVERFLOW);
		default: std::cout << "Unrecognized Exception, "; break;
	}
#undef CASE
}


static LONG WINAPI exceptionHandler(LPEXCEPTION_POINTERS info)
{
	// char porgramPath[MAX_PATH];
	// ::GetModuleFileNameA(nullptr, porgramPath, sizeof(porgramPath));

	printExceptionCode(info);
	std::cout.fill('0');
	std::cout << "fault address 0x" << std::hex << std::setw(8) <<
#ifdef PLATFORM_X64
		info->ContextRecord->Rip << std::dec << std::endl;
#else
		info->ContextRecord->Eip << std::dec << std::endl;
#endif

	// If this is a stack overflow then we can't walk the stack
	if (EXCEPTION_STACK_OVERFLOW != info->ExceptionRecord->ExceptionCode)
	{
		HANDLE process = ::GetCurrentProcess();
		if (SymInitialize(process, 0, true) == 0)
			std::cerr << "Failed to init symbol context" << std::endl;
		else
		{
			std::cout << "backtrace:" << std::endl;
			Singleton<Backtrace>::instance()->report(info->ContextRecord);
			::SymCleanup(process);
		}
	}

	// ::exit(EXIT_FAILURE);
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void installBacktrace()
{
#if 0
	(void)Singleton<Backtrace>::instance();
#endif
}

#elif defined(__linux__)

#include <execinfo.h>
#include <signal.h>
#include <err.h>
#include <cxxabi.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>


class Backtrace
{
	static unsigned char alternateStack[SIGSTKSZ];

	static void posixSignalHandler(int sig, siginfo_t *siginfo, void *);

	static void setupAlternateStack()
	{
		stack_t ss;
		ss.ss_sp = reinterpret_cast<void *>(alternateStack);
		ss.ss_size = SIGSTKSZ;
		ss.ss_flags = 0;

		if (::sigaltstack(&ss, nullptr) != 0)
			::err(1, "sigaltstack");
	}

	static void registerSignalHandlers()
	{
		struct sigaction sig_action;
		sig_action.sa_handler = 0;
		sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;
		sig_action.sa_restorer = nullptr;
		sig_action.sa_sigaction = Backtrace::posixSignalHandler;
		sigemptyset(&sig_action.sa_mask);

		if (sigaction(SIGSEGV, &sig_action, nullptr) != 0) err(1, "sigaction");
		if (sigaction(SIGFPE, &sig_action, nullptr) != 0) err(1, "sigaction");
		if (sigaction(SIGINT, &sig_action, nullptr) != 0) err(1, "sigaction");
		if (sigaction(SIGILL, &sig_action, nullptr) != 0) err(1, "sigaction");
		if (sigaction(SIGTERM, &sig_action, nullptr) != 0) err(1, "sigaction");
		if (sigaction(SIGABRT, &sig_action, nullptr) != 0) err(1, "sigaction");
	}

public:

	static void dump(FILE *out = stderr, unsigned int maxFrames = 62);

	/*static void dump()
	{
		void *buffer[255];
		const int calls = ::backtrace(buffer, sizeof(buffer) / sizeof(void *));
		::backtrace_symbols_fd(buffer, calls, 1);
		::exit(EXIT_FAILURE);
	}*/

	static void setSignalHandler()
	{
		setupAlternateStack();
		registerSignalHandlers();
	}
};

unsigned char Backtrace::alternateStack[SIGSTKSZ];


void Backtrace::posixSignalHandler(int sig, siginfo_t *siginfo, void *)
{
	fputs("-------------------\n", stderr);
	switch (sig)
	{
		case SIGSEGV:
			fputs("Caught SIGSEGV: Segmentation Fault\n", stderr);
			break;
		case SIGINT:
			fputs("Caught SIGINT: Interactive attention signal, (usually ctrl+c)\n", stderr);
			break;
		case SIGFPE:
			switch (siginfo->si_code)
			{
				case FPE_INTDIV:
					fputs("Caught SIGFPE: (integer divide by zero)\n", stderr);
					break;
				case FPE_INTOVF:
					fputs("Caught SIGFPE: (integer overflow)\n", stderr);
					break;
				case FPE_FLTDIV:
					fputs("Caught SIGFPE: (floating-point divide by zero)\n", stderr);
					break;
				case FPE_FLTOVF:
					fputs("Caught SIGFPE: (floating-point overflow)\n", stderr);
					break;
				case FPE_FLTUND:
					fputs("Caught SIGFPE: (floating-point underflow)\n", stderr);
					break;
				case FPE_FLTRES:
					fputs("Caught SIGFPE: (floating-point inexact result)\n", stderr);
					break;
				case FPE_FLTINV:
					fputs("Caught SIGFPE: (floating-point invalid operation)\n", stderr);
					break;
				case FPE_FLTSUB:
					fputs("Caught SIGFPE: (subscript out of range)\n", stderr);
					break;
				default:
					fputs("Caught SIGFPE: Arithmetic Exception\n", stderr);
					break;
			}
			break;
		case SIGILL:
			switch (siginfo->si_code) {
				case ILL_ILLOPC:
					fputs("Caught SIGILL: (illegal opcode)\n", stderr);
					break;
				case ILL_ILLOPN:
					fputs("Caught SIGILL: (illegal operand)\n", stderr);
					break;
				case ILL_ILLADR:
					fputs("Caught SIGILL: (illegal addressing mode)\n", stderr);
					break;
				case ILL_ILLTRP:
					fputs("Caught SIGILL: (illegal trap)\n", stderr);
					break;
				case ILL_PRVOPC:
					fputs("Caught SIGILL: (privileged opcode)\n", stderr);
					break;
				case ILL_PRVREG:
					fputs("Caught SIGILL: (privileged register)\n", stderr);
					break;
				case ILL_COPROC:
					fputs("Caught SIGILL: (coprocessor error)\n", stderr);
					break;
				case ILL_BADSTK:
					fputs("Caught SIGILL: (internal stack error)\n", stderr);
					break;
				default:
					fputs("Caught SIGILL: Illegal Instruction\n", stderr);
					break;
			}
			break;
		case SIGTERM:
			fputs("Caught SIGTERM: a termination request was sent to the program\n", stderr);
			break;
		case SIGABRT:
			fputs("Caught SIGABRT: usually caused by an abort() or assert()\n", stderr);
			break;
		default:
			break;
	}
	fputs("backtrace:\n", stderr);
	Backtrace::dump();
	fputs("-------------------\n", stderr);
	::exit(EXIT_FAILURE);
}


void Backtrace::dump(FILE *out, unsigned int maxFrames)
{
	// storage array for stack trace address data
	void *addrlist[maxFrames + 1];

	// retrieve current stack addresses
	int addrlen = backtrace(addrlist, (int)(sizeof(addrlist) / sizeof(void *)));

	if (addrlen == 0)
	{
		fprintf(out, "  <empty, possibly corrupt>\n");
		return;
	}

	// resolve addresses into strings containing "filename(function+address)",
	// this array must be free()-ed
	char **symbollist = backtrace_symbols(addrlist, addrlen);

	// allocate string which will be filled with the demangled function name
	size_t funcnamesize = 256;
	char *funcname = (char *)malloc(funcnamesize);

	// iterate over the returned symbol lines. skip the first, it is the
	// address of this function.
	for (int i = 1; i < addrlen; ++i)
	{
		int nameLength = 0;
		char *beginName = 0, *beginOffset = 0, *endOffset = 0;

		// find parentheses and +address offset surrounding the mangled name:
		// ./module(function+0x15c) [0x8048a6d]
		for (char *p = symbollist[i]; *p != 0; ++p)
		{
			++nameLength;
			if (*p == '(')
				beginName = p;
			else if (*p == '+')
				beginOffset = p;
			else if ((*p == ')' && beginOffset) ||
					 nameLength >= 62)
			{
				endOffset = p;
				break;
			}
		}

		printf("%02d: ", i);

		if (beginName && beginOffset && endOffset && beginName < beginOffset)
		{
			*beginName++ = '\0';
			*beginOffset++ = '\0';
			*endOffset = '\0';

			// mangled name is now in [beginName, beginOffset) and caller
			// offset in [beginOffset, endOffset). now apply __cxa_demangle():
			int status = 0;
			char *ret = abi::__cxa_demangle(beginName, funcname, &funcnamesize, &status);
			if (status == 0)
			{
				funcname = ret; // use possibly realloc()-ed string
				printf("%s: %s+%s\n", symbollist[i], funcname, beginOffset);
			}
			else
			{
				// demangling failed. Output function name as a C function with no arguments.
				printf("%s: %s()+%s\n", symbollist[i], beginName, beginOffset);
			}
		}
		else
		{
			// couldn't parse the line? print the whole line.
			printf("%s\n", symbollist[i]);
		}
	}

	free(funcname);
	free(symbollist);
}

/*
void boom()
{
	int a[10];
	printf("%d", a[-100000]);
}

void two()
{
	boom();
}

void one()
{
	two();
}

int main()
{
	Backtrace::setSignalHandler();
	// Backtrace::dump();
	one();
	return 0;
}
*/
void installBacktrace() { Backtrace::setSignalHandler(); }

#endif

#include <iostream>

extern "C" void __cxa_pure_virtual()
{
	std::cout << "__cxa_pure_virtual: pure virtual method called"
		<< std::endl << std::flush;
}
