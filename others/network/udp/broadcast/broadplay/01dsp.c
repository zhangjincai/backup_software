//gcc 01dsp.c -o dsppay
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define    DEVICE   "/dev/dsp"

int init_device(int speed,int channel,int fmt);

int main(int argc,char *argv[])
{
    char *wavfile;
    int   wavfd,dspfd,channel=2,speed=11025,fmt=16;

    if(argv[1]==NULL)
    {
       fprintf(stderr,"use:[%s][wavfile]\n",argv[0]);
       return 3;
    }
    wavfile=argv[1];

    dspfd=init_device(speed,channel,fmt);
    if(dspfd<0){
      fprintf(stderr,"init device failed\n");
      return 1;
    }
    wavfd=open(wavfile,O_RDONLY);
    if(wavfd==-1)
    {
       perror(wavfile);
       return 2;
    }
    //------------------进行播放--------------------
    int ret,size=channel*speed*fmt*2;
    char buf[size];
    while(1)
    {
       printf("decode......\n");
       ret=read(wavfd,buf,size);
       if(ret==0)   break;
       write(dspfd,buf,ret);
    }
    printf("play over.....\n");
    return 0;
}
int init_device(int speed,int channel,int fmt)
{
    int fd,ret;
    fd=open(DEVICE,O_WRONLY);
    if(fd==-1)
    {
       perror(DEVICE);
       return -1;
    }
    //--------------------------
    ret=ioctl(fd,SNDCTL_DSP_CHANNELS,&channel);
    if(ret==-1)
    {
       fprintf(stderr,"设置声道数失败!!!!\n");
       return -2;
    }
    //----------------------------
    ret=ioctl(fd,SNDCTL_DSP_SPEED,&speed);
    if(ret==-1)
    {
       fprintf(stderr,"设置采样率失败!!!!\n");
       return -3;
    }
    //------------------------
    ret=ioctl(fd,SNDCTL_DSP_SETFMT,&fmt);
    if(ret==-1)
    {
       fprintf(stderr,"设置位宽失败!!!!\n");
       return -3;
    }
    return fd;
}








