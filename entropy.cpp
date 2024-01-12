#include<windows.h>
#include<winnt.h>
#include<stdio.h>
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include<math.h>
double loga(double n, double base){
	return double(log(n)/log(base));
}
float get_entropy(char *ptr,int start,int end){
	if((end-start)==0){return 0;}
	double entropy=0;
	int length=end-start;
	for(int a=0;a<256;a++){
		int len=length;
		BYTE *p=(BYTE*)(ptr+start);
		int count=0;
		while(len--){
			if(*p++==a){
				count++;
			}
		}
		double p_c=double(double(count)/double(length));
		if(p_c>0)entropy += -double(p_c*loga(p_c,2));
	}
	return entropy;
}

int file_size(char* filename)  
{  
    FILE *fp=fopen(filename,"r");  
    if(!fp) return -1;  
    fseek(fp,0L,SEEK_END);  
    int size=ftell(fp);  
    fclose(fp);
    return size;  
}
int main(int argc,char *argv[],char* envp[]){
	if(argc!=2){
		printf("usage:");
		return 0;
	}
	printf("%s",argv[1]);
	int filesize=file_size(argv[1]);
	int fd=open(argv[1],O_RDONLY|O_BINARY); 
	void *p=malloc(filesize);
	BYTE *ptr=(BYTE*)p;
	read(fd, p, filesize);
	
	PIMAGE_DOS_HEADER pDos=(PIMAGE_DOS_HEADER)ptr;
	if(pDos->e_magic!=IMAGE_DOS_SIGNATURE){
		printf("no MZ");
		return -1;
	}
	
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS) (ptr + pDos->e_lfanew);
	
	IMAGE_FILE_HEADER pFileH=(IMAGE_FILE_HEADER)(pNtH->FileHeader);
	int numOfSections=pFileH.NumberOfSections;
	
	PIMAGE_SECTION_HEADER pSec= (PIMAGE_SECTION_HEADER)IMAGE_FIRST_SECTION(pNtH);
	
	while(numOfSections--){
		printf("\r\n%s",(char*)pSec->Name);
		int offset=pSec->PointerToRawData;
		int length=pSec->SizeOfRawData;
		float entropy=get_entropy((char*)ptr,offset,offset+length);
		printf("     %f    ",entropy);
		if(entropy>6.75)printf("packed");
		if(entropy<6.75 && entropy >6.5)printf("possibly packed");
		if(entropy<6.5)printf("not packed");
		pSec++;
	}
	return 0;
}
