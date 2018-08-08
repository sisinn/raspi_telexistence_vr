# 全天球カメラとVRゴーグルで、テレイグジスタンス体験(11)

[前ページ](./goggles.md) / [top](./top.md) / [次ページ](./calibration.md)  

## 11. 結合動作  

### ■モニター表示の左目/右目分離
---
バーテックスシェーダの演算で、点の位置とスクリーン画角を変更し、左目/右目それぞれ同じものを表示するように変更  

※当たり前だが、カメラが1眼なので、表示側も3Dにはならない。  
　仮に、カメラを2眼にしても、  
　カメラ側左右の水平軸と、視聴側左目/右目の水平軸が一致するとは限らないので、最初から考慮しない。  

### ■XYZ軸入れ替え
---
BNO055側クォータニオンのXYZ軸と、openFrameworks側クォータニオンのXYZ軸を一致させるように、適切に入れ替え。

ちゃんと理解できていないので、どれを入れ替えればいいのやら。。。  
以下のサイトを参考にしつつ  
* [UnityでiOSのジャイロの使い方をちゃんと説明する](https://qiita.com/fuqunaga/items/b1a3e38af71f062f0781)  

困ったときは、総当たりで地道に確認。  

※ofQuaternionの`set()`関数の引数は、左から順にfloat x, float y, float z, float w  

<span style="font-size: 200%">左から順にX、Y、Z、W</span>  

　  
<span style="font-size: 300%">左から順にX、Y、Z、W</span>  

　ここ間違ってて、苦しみました。。。  

### ■表示処理側アプリ作成
---
前項までの内容を踏まえて、再生側機器としてVRゴーグルにカメラ映像を描画するLinuxアプリを作成  
ソースコードは、GitHub参照  

Makeして動作確認
```
$ cd /home/pi/vr360_view
$ make
$ ./bin/vr360_view  -q w,_y,_x,_z -t 90 -f 10.0 -r 90 -s 0.02
```
※事前に撮影側機器を立ち上げて、送信開始しておく  

この時点で、10fps程度しか出ていない。性能改善が必要。。。  

### ■アプリの自動起動設定  
---
[Raspberry Piでプログラムを自動起動する5種類の方法を比較・解説](http://hendigi.karaage.xyz/2016/11/auto-boot/)  

※サービス起動の依存関係を以下のコマンドで前もって調べておく  
```
$ systemctl list-dependencies
```

#### 撮影機器側  
配信処理を実行する。シェルスクリプトを準備。  
```
$ cd /home/pi
$ touch vr360_sender.sh
$ chmod 755 vr360_sender.sh
$ vi vr360_sender.sh
```

vr360_sender.shの中身は以下の通り  
``` sh
#! /bin/sh

if [ -e /dev/video0 ]; then
    echo "Camera Data ... Send Start"
    ffmpeg -f v4l2 -input_format h264 -video_size 1920x960 -framerate 30 -i /dev/video0 -vcodec copy -an  -f mpegts udp://192.168.250.3:1234
else
    echo "Camera Not Detect"
fi
```

systemdに、配信処理をを登録  
```
$ cd /etc/systemd/system/
$ sudo vi vr360_sender.service
```

vr360_sender.serviceの中身は以下の通り  
```
[Unit]
Description=VR360_SENDER
After=networking.service hostapd.service rc-local.service systemd-journald.service

[Service]
Type=simple
WorkingDirectory=/home/pi
ExecStart=/home/pi/vr360_sender.sh
StandardOutput=journal
StandardError=journal

[Install]
WantedBy = multi-user.target
```

サービスの開始と起動登録
```
$ sudo systemctl start vr360_sender
$ sudo systemctl stop vr360_sender
$ journalctl -f -u vr360_sender.service
$ sudo systemctl enable vr360_sender.service
```

#### 表示機器側  
配信処理を実行する。シェルスクリプトを準備。  
```
$ cd /home/pi
$ touch vr360_receiver.sh
$ chmod 755 vr360_receiver.sh
$ vi vr360_receiver.sh
```

vr360_receiver.shの中身は以下の通り  
``` sh
#! /bin/sh

if [ -e /dev/ttyACM0 ]; then
    echo "VR View ... Start"
    /home/pi/vr360_view/bin/vr360_view  -q w,_y,_x,_z -t 90 -f 10.0 -r 90 -s 0.02
else
    echo "3DoF Sensor Not Detect"
fi
```

systemdに、配信処理をを登録  
```
$ cd /etc/systemd/system/
$ sudo vi vr360_receiver.service
```

vr360_receiver.serviceの中身は以下の通り  
```
[Unit]
Description=VR360_RECEIVER
After=networking.service rc-local.service systemd-journald.service

[Service]
Type=simple
WorkingDirectory=/home/pi
ExecStart=/home/pi/vr360_receiver.sh
StandardOutput=journal
StandardError=journal

[Install]
WantedBy = multi-user.target
```

サービスの開始と起動登録
```
$ sudo systemctl start vr360_receiver
$ sudo systemctl stop vr360_receiver
$ journalctl -f -u vr360_receiver.service
$ sudo systemctl enable vr360_receiver.service
```

### ■運用手順  
---
1. 撮影側ラズパイに、カメラを接続し、電源ON  
※WiFiアクセスポイントの起動で、しばらく（5秒ぐらい？）待つ必要あり  
2. フィッシングベストを着る  
3. ポケットに表示側ラズパイ、モバイルバッテリーを入れる  
4. VRゴーグルにモニター挿入  
5. 表示側ラズパイに、arduino＆モニターを接続し、電源ON  
6. VRゴーグル装着  
7. カメラ映像が表示されるのを待つ  
8. 3秒待つ  
9. 正面を向いて、赤スイッチを押す  
10. 3秒待つ  

[前ページ](./goggles.md) / [top](./top.md) / [次ページ](./calibration.md)  
