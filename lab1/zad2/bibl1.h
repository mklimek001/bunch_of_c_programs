extern void** createTable(int tabSize);
extern void fileStats(char* filePath, int pathLength);
extern char *readFromTempFile(char *tmp_file_name, int file_size);
extern void freeMemory(void *index);