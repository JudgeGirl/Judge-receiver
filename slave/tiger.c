#undef NDEBUG
#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include"config.h"

int main(int argc,char *argv[]) {
	static char b[1<<20];
	assert(argc>1);
	long long n=atoll(argv[1]);
	FILE *fp=fopen(SLAVE_OUT,"wb");
	assert(fp!=NULL);
	for(;;) {
		const int r=fread(b,1,sizeof(b),stdin);
		if(r<=0)break;
		if(n>=0&&(n-=r)<0) {
			fwrite(b,1,n+r,fp);
			return 1;
		}
		assert(fwrite(b,1,r,fp)==r);
	}
	assert(fclose(fp)==0);
	return 0;
}
