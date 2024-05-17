internal uint32
flen(FILE *f)
{
	fseek(f, 0, SEEK_END);
	uint32 flen = ftell(f);
	fseek(f, 0, 0);
	return flen;
}


