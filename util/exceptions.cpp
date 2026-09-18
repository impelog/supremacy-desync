#include "includes.h"

Exceptions g_exceptions{};

LONG WINAPI Exceptions::OnException( EXCEPTION_POINTERS* info ) {
    const auto code = info->ExceptionRecord->ExceptionCode;
    const auto address = info->ExceptionRecord->ExceptionAddress;

    std::ostringstream oss;
    oss << "[EXCEPTION HANDLER]\n";
    oss << "Code: 0x" << std::hex << std::uppercase << code << "\n";
    oss << "Address: 0x" << address << "\n";
    oss << "Description: " << g_exceptions.GetErrorDesc( code ) << "\n";
    oss << "----------------------------------------\n";

    HMODULE module = nullptr;
    if ( GetModuleHandleExA( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, static_cast< LPCSTR >( address ), &module ) && module ) {
        char path[ MAX_PATH ];
        if ( GetModuleFileNameA( module, path, MAX_PATH ) )
            oss << "Module: " << path << "\n";
    }

    oss << "----------------------------------------\n";

    const CONTEXT* ctx = info->ContextRecord;

    oss << std::hex << std::setfill( '0' );
    oss << "EAX: 0x" << std::setw( 8 ) << ctx->Eax
        << "  EBX: 0x" << std::setw( 8 ) << ctx->Ebx
        << "  ECX: 0x" << std::setw( 8 ) << ctx->Ecx
        << "  EDX: 0x" << std::setw( 8 ) << ctx->Edx << "\n";

    oss << "ESI: 0x" << std::setw( 8 ) << ctx->Esi
        << "  EDI: 0x" << std::setw( 8 ) << ctx->Edi
        << "  EBP: 0x" << std::setw( 8 ) << ctx->Ebp
        << "  ESP: 0x" << std::setw( 8 ) << ctx->Esp << "\n";

    oss << "EIP: 0x" << std::setw( 8 ) << ctx->Eip << "\n";

    oss << "----------------------------------------\n";

    if ( info->ExceptionRecord->NumberParameters > 0 ) {
        oss << "Exception Parameters: ";
        for ( DWORD i = 0; i < info->ExceptionRecord->NumberParameters; ++i )
            oss << "0x" << std::hex << info->ExceptionRecord->ExceptionInformation[ i ] << " ";
        oss << "\n";
    }
    else
        oss << "No Exception Parameters.\n";

    oss << "----------------------------------------\n";

    std::time_t now = std::time( nullptr );
    oss << "Time: " << std::put_time( std::localtime( &now ), "%Y-%m-%d %H:%M:%S" ) << "\n\n";

    MessageBoxA( nullptr, oss.str( ).c_str( ), "Unhandled Exception", MB_ICONERROR );

    // prevent the system from executing its own exception handler.
    return EXCEPTION_EXECUTE_HANDLER;
}
