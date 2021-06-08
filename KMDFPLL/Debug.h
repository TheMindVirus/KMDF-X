#ifndef DEBUG_H
#define DEBUG_H

#ifndef DEBUG_FILENAME
#define DEBUG_FILENAME   L"KMDFPLL.LOG"
#endif//DEBUG_FILENAME

#ifndef DEBUG_FILEROOT
#define DEBUG_FILEROOT   L"\\SystemRoot\\"
#endif//DEBUG_FILEROOT

#ifndef DEBUG_PATHSIZE
#define DEBUG_PATHSIZE   1024
#endif//DEBUG_PATHSIZE

#ifndef DBG
#define debug(...)   
#else
#define debug(...)   DebugLogToFile(__VA_ARGS__);

static BOOLEAN _DebugOverwrite = TRUE;

void DebugLogToFile(LPSTR format, ...)
{
	NTSTATUS status = STATUS_SUCCESS;

	char buffer[DEBUG_PATHSIZE] = "";
	va_list va;
	va_start(va, format);
	vsprintf(buffer, format, va);
	va_end(va);
	strcat(buffer, "\n");

	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, buffer));

	HANDLE file;
	UNICODE_STRING path;
	WCHAR pathbuffer[DEBUG_PATHSIZE];
	IO_STATUS_BLOCK result;
	OBJECT_ATTRIBUTES attributes;

	wcscpy(pathbuffer, DEBUG_FILEROOT);
	wcscat(pathbuffer, DEBUG_FILENAME);
	RtlInitUnicodeString(&path, pathbuffer);
	InitializeObjectAttributes(&attributes, &path, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	status = ZwCreateFile(&file, FILE_APPEND_DATA, &attributes, &result, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, 
		(_DebugOverwrite) ? FILE_OVERWRITE_IF : FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
	if (!NT_SUCCESS(status)) { return; }
	_DebugOverwrite = FALSE;

	status = ZwWriteFile(file, NULL, NULL, NULL, &result, buffer, (ULONG)strlen(buffer), NULL, NULL);
	for (ULONG i = 0; i < 10; ++i)
	{
		if (result.Status != STATUS_PENDING) { break; }
		KeStallExecutionProcessor(100);
	}
	ZwClose(file);
}

#endif//NDEBUG

#endif//DEBUG_H