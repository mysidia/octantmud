#ifdef __cplusplus
extern "C" {
#endif
	
const char *annotated_md5_hex(const char *passwd);
const char *md5_printable(unsigned char md5buffer[16]);
void md5_buffer(unsigned char *passwd, int len, unsigned char *target);

#ifdef __cplusplus
}
#endif
