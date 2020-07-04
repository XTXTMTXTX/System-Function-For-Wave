#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<cmath>
#include<algorithm>
#include<memory>
#include<iostream>
#include<assert.h>
#include"wave.h"

#define ABS(x) ((x)>0?(x):-(x))
using namespace std;
const double eps=1e-12;
enum{
	DISCRETE=1,
	CONTINUOUS=2
};
class Sys{
protected:
	double *a,*b,*w;
	int size;
public:
	void init(int r,double *num,double *den){
		size=r;
		a=(double*)malloc(sizeof(double)*(size+1));
		b=(double*)malloc(sizeof(double)*(size+1));
		memcpy(a,num,sizeof(double)*(size+1));
		memcpy(b,den,sizeof(double)*(size+1));
		w=(double*)malloc(sizeof(double)*(size+1));
		memset(w,0,sizeof(double)*(size+1));
	}
	virtual double process(double input)=0;
	Sys(){a=b=w=nullptr;}
	~Sys(){
		clear();
	}
	void clear(){
		if(w)free(w);
		if(a)free(a);
		if(b)free(b);
	}
};
class Discrete_Sys:public Sys{
public:
	double process(double input){
		for(int i=size;i>0;i--)w[i]=w[i-1];w[0]=input;
		for(int i=1;i<=size;i++)w[0]-=b[size-i]*w[i];
		w[0]/=b[size];
		static double output;
		output=0;
		for(int i=0;i<=size;i++)output+=a[size-i]*w[i];
		return output;
	}
};
class Continuous_Sys:public Sys{
protected:
	double freq;
public:
	Continuous_Sys(double f):freq(f){}
	double process(double input){
		static double tmp,wnew;
		tmp=wnew=b[0];
		for(int i=1;i<=size;i++){
			wnew=b[i]+tmp/2.0/freq;
			input-=w[size-i]*tmp/2.0/freq+w[size-i+1]*tmp;
			tmp=wnew;
		}
		tmp=wnew=input/wnew;
		for(int i=1;i<=size;i++){
			wnew=(w[i-1]+tmp)/2.0/freq+w[i];
			w[i-1]=tmp;
			tmp=wnew;
		}
		w[size]=wnew;
		static double output;
		output=0;
		for(int i=0;i<=size;i++)output+=a[size-i]*w[i];
		return output;
	}
};
int n,m,r,sys;
double *num,*den;
int main(int argc, char* argv[]){
	if(argc!=2){
		printf("Usage: %s WaveFile.\n",argv[0]);
		fflush(stdin);
		fflush(stdout);
		getchar();
		return 0;
	}
	char newfile[256];
	strcpy(newfile,argv[1]);
	strcat(newfile,".new.wav"); // generate new wave file name
	
	FILE *fpi=fopen(argv[1],"rb");
	assert(fpi);
	
	unique_ptr<Wave_header> Wave;
	Wave=make_unique<Wave_header>();
	fread(&(Wave->riff->fcc),sizeof(char),4,fpi);
	if(Wave->riff->fcc!=MakeFOURCC<'R','I','F','F'>::value){
		puts("Not a RIFF file.");
		fflush(stdin);
		fflush(stdout);
		getchar();
		return 0;
	}
	fread(&(Wave->riff->cb_size),sizeof(uint32_t),1,fpi);
	
	fread(&(Wave->wave_fcc),sizeof(char),4,fpi);
	if(Wave->wave_fcc!=MakeFOURCC<'W','A','V','E'>::value){
		puts("Not a WAVE file.");
		fflush(stdin);
		fflush(stdout);
		getchar();
		return 0;
	}
	fread(&(Wave->fmt->fcc),sizeof(char),4,fpi);
	fread(&(Wave->fmt->cb_size),sizeof(uint32_t),1,fpi);
	printf("fmt_size: %d\n",Wave->fmt->cb_size);
	fread(&(Wave->fmt_data->format_tag),sizeof(uint16_t),1,fpi);
	if(Wave->fmt_data->format_tag!=WAVE_FORMAT_PCM){
		puts("Not a PCM file.");
		fflush(stdin);
		fflush(stdout);
		getchar();
		return 0;
	}
	fread(&(Wave->fmt_data->channels),sizeof(uint16_t),1,fpi);
	printf("channels: %d\n",Wave->fmt_data->channels);
	fread(&(Wave->fmt_data->sample_per_sec),sizeof(uint32_t),1,fpi);
	printf("sample_per_sec: %d\n",Wave->fmt_data->sample_per_sec);
	fread(&(Wave->fmt_data->bytes_per_sec),sizeof(uint32_t),1,fpi);
	printf("bytes_per_sec: %d\n",Wave->fmt_data->bytes_per_sec);
	fread(&(Wave->fmt_data->block_align),sizeof(uint16_t),1,fpi);
	printf("block_align: %d\n",Wave->fmt_data->block_align);
	fread(&(Wave->fmt_data->bits_per_sample),sizeof(uint16_t),1,fpi);
	printf("bits_per_sample: %d\n",Wave->fmt_data->bits_per_sample);
	if(Wave->fmt->cb_size!=16)fread(&(Wave->fmt_data->ex_size),sizeof(uint16_t),1,fpi);
	printf("ex_size: %d\n",Wave->fmt_data->ex_size);
	fseek(fpi,Wave->fmt_data->ex_size,SEEK_CUR);
	fread(&(Wave->data->fcc),sizeof(char),4,fpi);
	if(Wave->data->fcc!=MakeFOURCC<'d','a','t','a'>::value){
		puts("Data not find.");
		fflush(stdin);
		fflush(stdout);
		getchar();
		return 0;
	}
	fread(&(Wave->data->cb_size),sizeof(uint32_t),1,fpi);
	assert(Wave->fmt_data->block_align==Wave->fmt_data->channels*Wave->fmt_data->bits_per_sample/8);
	assert(Wave->data->cb_size%Wave->fmt_data->block_align==0);
	
	puts("(1) Discrete System");
	puts("(2) Continuous System");
	printf("Select: ");scanf("%d",&sys);
	unique_ptr<Sys> DS[Wave->fmt_data->channels];
	if(sys==DISCRETE){
		for(int i=0;i<Wave->fmt_data->channels;i++)DS[i]=make_unique<Discrete_Sys>();
	}else if(sys==CONTINUOUS){
		for(int i=0;i<Wave->fmt_data->channels;i++)DS[i]=make_unique<Continuous_Sys>(Wave->fmt_data->sample_per_sec);
	}else{
		puts("Input a valid number.");
		fflush(stdin);
		fflush(stdout);
		getchar();
		return 0;
	}
	
	
	printf("The order of numerator: ");scanf("%d",&n);
	printf("The order of denominator: ");scanf("%d",&m);
	r=max(n,m);
	num=(double*)malloc(sizeof(double)*(r+1));
	den=(double*)malloc(sizeof(double)*(r+1));
	memset(num,0,sizeof(double)*(r+1));
	memset(den,0,sizeof(double)*(r+1));
	printf("The coefficients of numerator: ");
	for(int i=n;i>=0;i--)scanf("%lf",num+i);
	printf("The coefficients of denominator: ");
	for(int i=m;i>=0;i--)scanf("%lf",den+i);
	while(n&&ABS(num[n])<eps)n--;
	while(m&&ABS(den[m])<eps)m--;
	r=m;
	if(n>m||ABS(den[m])<eps){
		puts("The order of denominator must be no less than the order of numerator.");
		fflush(stdin);
		fflush(stdout);
		getchar();
		return 0;
	}
	if(sys==DISCRETE){
		printf("\n\n");
		for(int i=r;i>=0;i--)if(ABS(num[i])>eps)i==r?printf("%lf ",num[i]):printf("%lf*z^%d ",num[i],i-r);
		printf("\n--------------------------\n");
		for(int i=r;i>=0;i--)if(ABS(den[i])>eps)i==r?printf("%lf ",den[i]):printf("%lf*z^%d ",den[i],i-r);
		printf("\n\n");
	}else{
		printf("\n\n");
		for(int i=r;i>=0;i--)if(ABS(num[i])>eps)i==0?printf("%lf ",num[i]):printf("%lf*s^%d ",num[i],i);
		printf("\n--------------------------\n");
		for(int i=r;i>=0;i--)if(ABS(den[i])>eps)i==0?printf("%lf ",den[i]):printf("%lf*s^%d ",den[i],i);
		printf("\nSample time: %e s\n",1.0/Wave->fmt_data->sample_per_sec);
	} // print sys
	
	
	for(int i=0;i<Wave->fmt_data->channels;i++)DS[i]->init(r,num,den);
	
	
	FILE *fpo=fopen(newfile,"wb");
	assert(fpo);
	
	Wave->riff->cb_size-=Wave->fmt_data->ex_size;
	Wave->fmt->cb_size-=Wave->fmt_data->ex_size;
	Wave->fmt_data->ex_size=0;
	fwrite(&(Wave->riff->fcc),sizeof(char),4,fpo);
	fwrite(&(Wave->riff->cb_size),sizeof(uint32_t),1,fpo);
	fwrite(&(Wave->wave_fcc),sizeof(char),4,fpo);
	fwrite(&(Wave->fmt->fcc),sizeof(char),4,fpo);
	fwrite(&(Wave->fmt->cb_size),sizeof(uint32_t),1,fpo);
	fwrite(&(Wave->fmt_data->format_tag),sizeof(uint16_t),1,fpo);
	fwrite(&(Wave->fmt_data->channels),sizeof(uint16_t),1,fpo);
	fwrite(&(Wave->fmt_data->sample_per_sec),sizeof(uint32_t),1,fpo);
	fwrite(&(Wave->fmt_data->bytes_per_sec),sizeof(uint32_t),1,fpo);
	fwrite(&(Wave->fmt_data->block_align),sizeof(uint16_t),1,fpo);
	fwrite(&(Wave->fmt_data->bits_per_sample),sizeof(uint16_t),1,fpo);
	if(Wave->fmt->cb_size!=16)fwrite(&(Wave->fmt_data->ex_size),sizeof(uint16_t),1,fpo);
	fwrite(&(Wave->data->fcc),sizeof(char),4,fpo);
	fwrite(&(Wave->data->cb_size),sizeof(uint32_t),1,fpo);
	for(uint32_t i=0;i<Wave->data->cb_size/Wave->fmt_data->block_align;i++){
		for(int ch=0;ch<Wave->fmt_data->channels;ch++){
			int data=0;
			
			fread(&data,Wave->fmt_data->bits_per_sample/8,1,fpi);
			if(data>>(Wave->fmt_data->bits_per_sample-1))data|=(-1)^((1<<(Wave->fmt_data->bits_per_sample))-1); // deal with negative numbers
			double tmp=data;
			tmp=DS[ch]->process(tmp);
			tmp=round(tmp)+eps;
			if(tmp>(1<<(Wave->fmt_data->bits_per_sample-1))-1)data=(1<<(Wave->fmt_data->bits_per_sample-1))-1; // deal with overflow
			else if(-tmp>(1<<(Wave->fmt_data->bits_per_sample-1))-1)data=-(1<<(Wave->fmt_data->bits_per_sample-1))+1;
			else data=tmp;
			fwrite(&data,Wave->fmt_data->bits_per_sample/8,1,fpo);
		}
	}
	
	
	for(int i=0;i<Wave->fmt_data->channels;i++)DS[i].reset();
	Wave.reset();
	free(num);
	free(den);
	fclose(fpi);
	fclose(fpo);
	puts("Finished.");
	fflush(stdin);
	fflush(stdout);
	getchar();
	return 0;
}
// samples
/*
1
2
2
0.03333 0 0
1 -1.344 0.9025
*/
/*
2
1
1
1 0
1 7.109e4
*/
