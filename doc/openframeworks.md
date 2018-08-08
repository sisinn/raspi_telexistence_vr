# 全天球カメラとVRゴーグルで、テレイグジスタンス体験(7)

[前ページ](./h264.md) / [top](./top.md) / [次ページ](./vr360.md)  

## 7. openFrameworksの導入

動画データとOpenGL（ES）を容易に扱えそうなプラットフォームとして、openFrameworksを導入する。  

openFrameworksの概要は、以下のサイトを参照させていただいた  
[openFrameworks 入門編](https://www.ei.tohoku.ac.jp/xkozima/lab/ofTutorial1.html)

### ■インストール
---
[Getting your Raspberry Pi ready for openFrameworks](https://openframeworks.cc/setup/raspberrypi/raspberry-pi-getting-started/)  

※上記手順通りでは正しく導入できないので、少し変更  

最新のv0.10系を入れる  
openFrameworksの古いバージョンは、RaspbianのStretch版に対応していないので注意  
```
$ wget --no-check-certificate https://openframeworks.cc/versions/v0.10.0/of_v0.10.0_linuxarmv6l_release.tar.gz
$ mkdir openFrameworks
$ tar vxfz of_v0.10.0_linuxarmv6l_release.tar.gz -C openFrameworks --strip-components 1
$ cd /home/pi/openFrameworks/scripts/linux/debian
$ sudo ./install_dependencies.sh
$ sudo ./install_codecs.sh
$ sudo make -j2 -B Release -C /home/pi/openFrameworks/libs/openFrameworksCompiled/project 
```
※install_*.shは、途中で「Y/n」の入力を何回か求められる  
※makeは30分程度かかるので、放置して別の作業をする。  
※makeのオプションを"-j4"にすると、ビルドに失敗する場合がある  

GPUに割り当てるメモリを変更  
```
$ sudo raspi-config
```
[7 Advanced Options] → [A3 Memory Split]  
512MBを割り当てて、再起動  

### ■サンプル動作確認  
---
本体のmakeが終わった後、  
サンプルが大量に同封されているので、どれかをmakeして動かしてみる。  
```
$ cd /home/pi/openFrameworks/examples/graphics/polygonExample
$ make
$ ./bin/polygonExample
```
※リモート接続のTeraTerm側コンソールで実行しても、表示はモニター側に出力されるので注意  

### ■アドオン  
---
* 動画再生用に「ofxOMXPlayey」  
https://github.com/jvcleave/ofxOMXPlayer  
* Arduinoとのシリアル通信用に「ofxSerial」「ofxIO」を入れる。  
https://github.com/bakercp/ofxIO  
https://github.com/bakercp/ofxSerial  
※標準に付属している「ofSerial」は使わないこと  
※arduino側でfirmataを使用する場合は、「ofArduino」という選択肢がある  

#### ofxOMXPlayeyのインストール  

```
$ cd /home/pi/openFrameworks/addons
$ git clone https://github.com/jvcleave/ofxOMXPlayer.git
$ cd ofxOMXPlayer
$ git checkout master
$ sudo ./install_depends.sh
```

※stableなブランチがないので、Masterを入れる。  
　今回動作確認したのは、以下のコミット  
　最新版でないと、いろいろと課題発生する  
```
commit cc9e3b7b55c94104de40c18b79c2555c2d76697f
Merge: 1158556 b059d81
Author: Jason Van Cleave <jvcleave@gmail.com>
Date:   Wed Jun 6 13:58:07 2018 -0400

    Merge pull request #128 from jvcleave/pkgconfig

    Pkgconfig
```

動作確認  
```
$ cd /home/pi/openFrameworks/addons/ofxOMXPlayer/example-basic
$ make
$ ./bin/example-basic
```
※makeは30分程度かかるので、放置して別の作業をする。  
※RaspbianのLite版では、エラーが出てうまく動作しなかった。。。  

動くことは動く。しかし、ちょっと遅い。17fpsぐらいしか出ていない。  
他のサイトの記事では、問題ない感じだったので、「Raspberry Pi 3 Model B+」か「Raspbian Strech版」だけの課題かも・・・  
CPU性能として10%程度しか消費していないので、ちゃんとハード支援を使えていると思うのだが。。。  

参考：[raspberry piでofxOMXPlayerに違うビットレートの動画を読ませてみた](http://k-s-m-r.blogspot.com/2015/11/raspberry-piofxomxplayer.html)  

#### ofxIO、ofxSerialのインストール  
こちらも、stableブランチを使用  
```
$ cd /home/pi/openFrameworks/addons
$ git clone https://github.com/bakercp/ofxIO.git
$ cd ofxIO
$ git checkout stable
$ cd ~/openFrameworks/addons
$ git clone https://github.com/bakercp/ofxSerial.git
$ cd ofxSerial
$ git checkout stable
```

ofxIOの現時点でのstable  
```
commit 227d894ebea7d9ee22e54ee63707c34ab8e7fb1f
Author: Christopher Baker <me@christopherbaker.net>
Date:   Mon Jun 25 23:47:02 2018 -0600

    Update README.
```

ofxSerialの現時点でのstable  
```
commit 4c93f1f1305c41428a8fdec32b2deeebeb2ca08b
Author: Christopher Baker <me@christopherbaker.net>
Date:   Fri Jun 1 23:06:46 2018 -0500

    Further clean up.
```

### ■メモ  
---
* 処理を記載するソースコードは、`main.cpp`、`ofApp.cpp`、`ofApp.h`の３ファイル構成  
* `main.cpp`及びその内部の`main()`関数は、普通のUnixアプリと同じ扱い  
* 使用するアドオンを`addons.make`に記載する。  
    ※今回使用するのは４つofxOMXPlayer、ofxIO、ofxPoco、ofxSerial  
* `Makefile`で、openFrameworksの全体ツリーがある場所を指定する。  
    ※今回は、`/home/pi/openFrameworks/`  

---
* ラズパイは、OpenGL ES 2.0なので、`ofGLESWindowSettings`で`setGLESVersion(2)`を設定  

---  
* `ofApp.cpp`での処理を記載する主要な関数は３つ：`ofApp::setup()`、`ofApp::update()`、`ofApp::draw()`  

---
* `ofSetVerticalSync(true)`：モニターの描画に同期させる。ティアリングが起きないようにする。  
* `//ofEnableDepthTest()`：OpenGL空間の奥行方向の重なり合いの調整をしない  
* `ofHideCursor()`：カーソルを描画しない  
* `ofSetFrameRate(25)`：フレームレートを、一旦25Hzに設定  
    ※適切な値は、何になるだろう？  

|処理場所|フレームレート|  
|:---:|:---:|  
|カメラ|不明|  
|UVC(V4L2)|30Hz|  
|送信側処理|30Hz|  
|動画再生処理|30Hzは無理そう|  
|opneFrameworks|25Hz|  
|モニター|50.28Hz|  

---
* `std::printf()`：デバッグ用にコンソールにメッセージを出力  
* `ofPushMatrix()`、`ofPopMatrix()`で、処理の原点を変更できる。  
* `ofQuaternion`で、クォータニオンを直接扱える。ありがたや。  

[前ページ](./ffmpeg.md) / [top](./top.md) / [次ページ](./vr360.md)  
