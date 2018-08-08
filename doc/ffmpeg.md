# 全天球カメラとVRゴーグルで、テレイグジスタンス体験(5)

[前ページ](./receiver.md) / [top](./top.md) / [次ページ](./h264.md)  

## 5. FFmpegの導入

Linux上でメディアデータを扱うために、FFmpegを導入する。

### ■FFmpeg
---
FFmpegは、いろいろと紆余曲折あって大人の事情で、Raspbianに標準では入っていない。  
また、apt-getでインストールできるものは、ラズパイのハード支援機能を使う設定になっていないので、ソースコードからビルドしなおす。  
さらに問題が複雑化していて、最新バージョンでは、再生側のハード支援が動いていないみたい。

[RPI-3: Performance issue on FFmpeg with mmal support](https://raspberrypi.stackexchange.com/questions/66923/rpi-3-performance-issue-on-ffmpeg-with-mmal-support)  
→ 3.1.10までは、OKとのこと

|バージョン|用途|
|:---:|:---:|
|3.1系|FFmpeg付属のffplayで再生させる場合|
|3.4系|1つ前のバージョン|
|4.0系|最新（2018/08/01時点）|

※ffplayを動作させるにはSDLが必要  
　3.1系は「SDL1.2」、3.2系以降は「SDL2」  
　この変更で、ハード支援が動かなくなったんでは？・・・  
※apt-getで取得できるSDLは、X-Windows用のビデオドライバーが必要？  
　RaspbianのLite版では、SDL動かない。。。  

#### ビルド手順（4.0系の場合）  
ビルド手順は、以下のサイトを参照させていただきました。  
[Rapberry Pi 3 のハードウエアエンコーダh264_omxが使えるFFmpegをビルドする](
https://signal-flag-z.blogspot.com/2016/09/rapberry-pi-3-h264omxffmpeg.html)  

オーディオの使用予定はないが、一応オーディオ対応させておく。  
まずは、ALSAライブラリの導入  
```
$ cd /home/pi
$ wget ftp://ftp.alsa-project.org/pub/lib/alsa-lib-1.1.6.tar.bz2
$ tar xjvf alsa-lib-1.1.6.tar.bz2
$ cd alsa-lib-1.1.6
$ ./configure --prefix=/home/pi/alsa-lib
$ make -j4
$ make install
```

次にFFmpegの導入  
```
$ cd /home/pi
$ git clone git://source.ffmpeg.org/ffmpeg.git
$ cd ffmpeg
$ git checkout n4.0.2
$ sudo apt-get install -y libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
$ sudo apt-get install -y libfontconfig1-dev libfribidi-dev
$ sudo apt-get install -y libomxil-bellagio-dev
$ ./configure --enable-gpl  --enable-nonfree --enable-mmal \
 --enable-omx-rpi --enable-omx  --enable-opengl --enable-libfreetype \
 --enable-libfontconfig --enable-libfribidi \
 --extra-cflags="-I/home/pi/alsa-lib/include" --extra-ldflags="-L/home/pi/alsa-lib/lib" \
 --extra-libs=-ldl
$ make -j4
$ sudo make install
```
※FFmpegに対するmake実行は、かなり時間かかる（40分ぐらい？）  

#### ビルド手順（3.4系の場合）  
ALSAライブラリの導入  
```
$ cd /home/pi
$ wget ftp://ftp.alsa-project.org/pub/lib/alsa-lib-1.1.6.tar.bz2
$ tar xjvf alsa-lib-1.1.6.tar.bz2
$ cd alsa-lib-1.1.6
$ ./configure --prefix=/home/pi/alsa-lib
$ make -j4
$ make install
```

次にFFmpegの導入  
```
$ cd /home/pi
$ git clone git://source.ffmpeg.org/ffmpeg.git
$ cd ffmpeg
$ git checkout n3.4.4
$ sudo apt-get install -y libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
$ sudo apt-get install -y libfontconfig1-dev libfribidi-dev
$ sudo apt-get install -y libomxil-bellagio-dev
$ ./configure --enable-gpl  --enable-nonfree --enable-mmal \
 --enable-omx-rpi --enable-omx  --enable-opengl --enable-libfreetype \
 --enable-libfontconfig --enable-libfribidi \
 --extra-cflags="-I/home/pi/alsa-lib/include" --extra-ldflags="-L/home/pi/alsa-lib/lib" \
 --extra-libs=-ldl
$ make -j4
$ sudo make install
```

#### ビルド手順（3.1系の場合）  
ALSAライブラリの導入  
```
$ cd /home/pi
$ wget ftp://ftp.alsa-project.org/pub/lib/alsa-lib-1.1.3.tar.bz2
$ tar xjvf alsa-lib-1.1.3.tar.bz2
$ cd alsa-lib-1.1.3
$ ./configure --prefix=/home/pi/alsa-lib
$ make -j4
$ make install
```

次にFFmpegの導入  
```
$ cd /home/pi
$ git clone git://source.ffmpeg.org/ffmpeg.git
$ cd ffmpeg
$ git checkout n3.1.11
$ sudo apt-get install -y libsdl1.2-dev
$ sudo apt-get install -y libfontconfig1-dev libfribidi-dev
$ sudo apt-get install -y libomxil-bellagio-dev
$ ./configure --enable-gpl  --enable-nonfree --enable-mmal \
 --enable-omx-rpi --enable-omx  --enable-opengl --enable-libfreetype \
 --enable-libfontconfig --enable-libfribidi \
 --extra-cflags="-I/home/pi/alsa-lib/include" --extra-ldflags="-L/home/pi/alsa-lib/lib" \
 --extra-libs=-ldl
$ make -j4
$ sudo make install
```

[前ページ](./receiver.md) / [top](./top.md) / [次ページ](./h264.md)  
