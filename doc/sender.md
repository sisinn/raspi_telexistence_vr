# 全天球カメラとVRゴーグルで、テレイグジスタンス体験(3)

[前ページ](./raspberry_init.md) / [top](./top.md) / [次ページ](./receiver.md)  

## 3. 撮影側のラズパイ環境構築

insta360 AirからUSB経由で映像データを取得し、ネットワークに流す。  

### ■insta360 Airの準備  
---
USBのフレキシブルケーブル、microUSB<->USB変換冶具を使って、ラズパイに、insta360 Airを接続
ラズパイ側からは、`/dev/video0`として見える。  

### ■v4l-utilsをインストール  
---  
```  
$ sudo apt-get install -y v4l-utils libv4l-dev
```  

※接続機器の確認
```  
$ v4l2-ctl --list-device
```  
以下の内容が表示されるはず
```  
Insta360 Air: Video Control (usb-3f980000.usb-1.1.2):
        /dev/video0
```  

※UVCフォーマットの確認
```  
$ v4l2-ctl --list-formats-ext
```  

以下の内容が表示されるはず
```  
ioctl: VIDIOC_ENUM_FMT
        Index       : 0
        Type        : Video Capture
        Pixel Format: 'MJPG' (compressed)
        Name        : Motion-JPEG
                Size: Discrete 1472x736
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 2176x1088
                        Interval: Discrete 0.033s (30.000 fps)
                Size: Discrete 3008x1504
                        Interval: Discrete 0.033s (30.000 fps)

        Index       : 1
        Type        : Video Capture
        Pixel Format: 'H264' (compressed)
        Name        : H.264
                Size: Discrete 1920x960
                        Interval: Discrete 0.033s (30.000 fps)
                        Interval: Discrete 0.042s (24.000 fps)
                        Interval: Discrete 0.050s (20.000 fps)
                Size: Discrete 2560x1280
                        Interval: Discrete 0.033s (30.000 fps)
                        Interval: Discrete 0.042s (24.000 fps)
                        Interval: Discrete 0.050s (20.000 fps)
```  

※Insta360 Airの各種フォーマットと画質の関係は、以下を参照  
[Insta360 Airの360度USBウェブカメラ(UVC)としての性能はなかなか優秀](http://hammmm.hatenablog.com/entry/2017/04/23/150552)

### ■mjpg-streamerの導入  
---
低遅延でリアルタイムに処理できることを優先するならば、
UVCから取得したMotionJPEGデータを、そのまま配信するのがベスト  
「mjpg-streamer」を使用する  
<https://github.com/jacksonliam/mjpg-streamer>  
```
$ sudo apt-get install -y libjpeg8-dev
$ git clone https://github.com/jacksonliam/mjpg-streamer.git
$ cd mjpg-streamer/mjpg-streamer-experimental
$ make
$ sudo make install
```

※GitHub上のmjpg-streamerツリーのブランチがMasterしかない。  
　今回使ったのは、以下のコミット  
```  
commit f387bb44e6c087271b763b27da998bf2e06c4f5d
Author: rhobincu <radu.hobincu@upb.ro>
Date:   Sun Jun 24 13:33:34 2018 +0300

    Updated readme to clarify that output_rtsp is incomplete. (#139)
```  

### ■動作確認  
---
Insta360 Airの映像を配信し、WindowsPC上のWebブラウザで確認  

ラズパイ上で以下を実行  
※-i -o -wオプションで使用するフォルダ指定は、相対アドレスも可能  
　後で設定する自動機能の場合を考慮して、絶対アドレスで指定しておく  
```
$ mjpg_streamer -i "/home/pi/mjpg-streamer/mjpg-streamer-experimental/input_uvc.so -d /dev/video0 -r 1472x736" -o "/home/pi/mjpg-streamer/mjpg-streamer-experimental/output_http.so -w /home/pi/mjpg-streamer/mjpg-streamer-experimental/www -p 8080"
```

* -rオプションは解像度  
    ※Insta360 Airの場合、「1472x736」、「2176x1088」、「3008x1504」の３つが選択可能  
* -yオプションは不要  
    ※Insta360 Air側でMotionJPEGに圧縮した状態で、USBからデータが入力されるため  
* 「mjpg-streamer」起動後、”UVCIOC_CTRL_ADD”および”UVCIOC_CTRL_MAP”でエラー表示が出るが、今回は使わないので放置  

WindowsPC上でブラウザを開き、以下のURLにアクセス  
※ここでは、ラズパイ側のIPアドレス=192.168.11.113としての場合を例示  
```
http://192.168.11.113:8080/
```

結果、  
Dual Fisheye画像なので見にくいが、目視レベルで非常に遅延少ない状態で動作していることを確認  

mjpg_streamerを終了するときは、Ctrl+Cで  

### ■性能確認  
---

コマンド一覧は、以下を参照  
[Linux負荷監視コマンドまとめ](https://qiita.com/aosho235/items/c4d6995743dd1dac16e1)  
[Linuxリソースの監視・確認に関するコマンド逆引きメモ](https://qiita.com/rsooo/items/42f0902d42bab6ecf175)  

---
[CPUをコアごとに確認する](https://qiita.com/shohhei1126/items/4b54290a3604f63f52c8)  
```
$ sudo apt-get install -y sysstat
$ sudo mpstat -P ALL
```

---
[iftopコマンドの使い方](https://qiita.com/hana_shin/items/bbf8e2f705d0237874b3)  
```
$ sudo apt-get install -y iftop
$ sudo iftop -i eth0 -t
```

---
mjpg-streamerは、受信側が誰もいないときは配信しないようなので、WindowsPC上のブラウザで映像を表示した状態で性能測定する。  
mjpg-streamer起動後、TeraTermをもう一つ開き、ラズパイに接続して測定  

#### 解像度＝1472x736の場合  

＜CPU性能＞  
```  
Linux 4.14.58-v7+ (raspberrypi)         01/08/18        _armv7l_        (4 CPU)

13:14:10     CPU    %usr   %nice    %sys %iowait    %irq   %soft  %steal  %guest  %gnice   %idle
13:14:10     all    0.50    0.00    0.36    0.16    0.00    0.05    0.00    0.00    0.00   98.92
13:14:10       0    0.40    0.00    0.45    0.28    0.00    0.21    0.00    0.00    0.00   98.67
13:14:10       1    0.30    0.00    0.32    0.18    0.00    0.00    0.00    0.00    0.00   99.20
13:14:10       2    0.65    0.00    0.37    0.11    0.00    0.00    0.00    0.00    0.00   98.86
13:14:10       3    0.66    0.00    0.32    0.08    0.00    0.01    0.00    0.00    0.00   98.92
```  

＜ネットワークトラフィック＞  
```  
   # Host name (port/service if enabled)            last 2s   last 10s   last 40s cumulative
--------------------------------------------------------------------------------------------
   1 raspberrypi                              =>     75.7Mb     75.0Mb     69.4Mb      104MB
     DESKTOP-SISINN                           <=      683Kb      677Kb      625Kb      938KB
   2 raspberrypi                              =>         0b        59b       193b       290B
     router.asus.com                          <=         0b        84b       269b       403B
   3 raspberrypi                              =>         0b        61b        51b        76B
     ntp-b2.nict.go.jp                        <=         0b        61b        51b        76B
--------------------------------------------------------------------------------------------
Total send rate:                                     75.7Mb     75.0Mb     69.4Mb
Total receive rate:                                   683Kb      677Kb      626Kb
Total send and receive rate:                         76.3Mb     75.6Mb     70.0Mb
--------------------------------------------------------------------------------------------
Peak rate (sent/received/total):                     75.8Mb      687Kb     76.5Mb
Cumulative (sent/received/total):                     104MB      938KB      105MB
============================================================================================
```  

#### 解像度＝2176x1088の場合  

＜CPU性能＞  
```  
Linux 4.14.58-v7+ (raspberrypi)         01/08/18        _armv7l_        (4 CPU)

13:20:52     CPU    %usr   %nice    %sys %iowait    %irq   %soft  %steal  %guest  %gnice   %idle
13:20:52     all    0.57    0.00    0.50    0.12    0.00    0.31    0.00    0.00    0.00   98.50
13:20:52       0    1.02    0.00    1.19    0.22    0.00    1.26    0.00    0.00    0.00   96.31
13:20:52       1    0.23    0.00    0.25    0.14    0.00    0.00    0.00    0.00    0.00   99.38
13:20:52       2    0.52    0.00    0.33    0.08    0.00    0.02    0.00    0.00    0.00   99.04
13:20:52       3    0.52    0.00    0.26    0.06    0.00    0.02    0.00    0.00    0.00   99.14
```  

＜ネットワークトラフィック＞  
```  
   # Host name (port/service if enabled)            last 2s   last 10s   last 40s cumulative
--------------------------------------------------------------------------------------------
   1 raspberrypi                              =>     87.2Mb     84.9Mb     84.9Mb      106MB
     DESKTOP-SISINN                           <=      779Kb      757Kb      757Kb      946KB
   2 raspberrypi                              =>         0b       173b       173b       216B
     router.asus.com                          <=         0b       238b       238b       298B
--------------------------------------------------------------------------------------------
Total send rate:                                     87.2Mb     84.9Mb     84.9Mb
Total receive rate:                                   779Kb      757Kb      757Kb
Total send and receive rate:                         88.0Mb     85.7Mb     85.7Mb
--------------------------------------------------------------------------------------------
Peak rate (sent/received/total):                     87.3Mb      780Kb     88.1Mb
Cumulative (sent/received/total):                     106MB      947KB      107MB
============================================================================================
```  

#### 解像度＝3008x1504の場合  

＜CPU性能＞  
```  
Linux 4.14.58-v7+ (raspberrypi)         01/08/18        _armv7l_        (4 CPU)

13:22:22     CPU    %usr   %nice    %sys %iowait    %irq   %soft  %steal  %guest  %gnice   %idle
13:22:22     all    0.61    0.00    0.53    0.12    0.00    0.38    0.00    0.00    0.00   98.37
13:22:22       0    1.21    0.00    1.36    0.21    0.00    1.54    0.00    0.00    0.00   95.69
13:22:22       1    0.25    0.00    0.27    0.13    0.00    0.02    0.00    0.00    0.00   99.34
13:22:22       2    0.51    0.00    0.32    0.08    0.00    0.02    0.00    0.00    0.00   99.06
13:22:22       3    0.50    0.00    0.25    0.06    0.00    0.02    0.00    0.00    0.00   99.17
```  

＜ネットワークトラフィック＞  
```  
   # Host name (port/service if enabled)            last 2s   last 10s   last 40s cumulative
--------------------------------------------------------------------------------------------
   1 raspberrypi                              =>     90.9Mb     88.7Mb     86.7Mb      152MB
     DESKTOP-SISINN                           <=      823Kb      801Kb      782Kb     1.34MB
   2 raspberrypi                              =>         0b         0b       123b       216B
     router.asus.com                          <=         0b         0b       170b       298B
--------------------------------------------------------------------------------------------
Total send rate:                                     90.9Mb     88.7Mb     86.7Mb
Total receive rate:                                   823Kb      801Kb      782Kb
Total send and receive rate:                         91.7Mb     89.5Mb     87.4Mb
--------------------------------------------------------------------------------------------
Peak rate (sent/received/total):                     91.1Mb      823Kb     91.9Mb
Cumulative (sent/received/total):                     152MB     1.34MB      153MB
============================================================================================
```  

---
カメラ側で圧縮したMotionJPEGを転送しているだけなので、CPUはほとんど使っていない。  
さすがに、ネットワーク帯域は消費しすぎ。  

### ■WiFi設定（親機）  
---
[最新ラズパイ「Raspberry Pi 3 Model B+」はココが違う！従来モデルからの進化点を検証](https://www.phileweb.com/review/article/201803/27/2984.html)  

* 「Raspberry Pi 3 B+」は、WiFi用のチップが、Cypress CYW43455に置き換えられ、デュアルバンド802.11ac（5GHz帯）を利用できる。  
* 有線LANについては、EthenetとUSBがバスを共有する構造。USB-LANチップが最大1GHzのLAN7515に置き換わった。BCM2837B0内部のインターフェイス仕様によりスループットは最大300Mbps  


|規格|無線電波の帯域|最大リンク速度|  
|:---:|:---:|:---:|  
|IEEE 802.11a|5GHz帯|54Mbps|  
|IEEE 802.11b|2.4GHz帯|11Mbps|  
|IEEE 802.11g|2.4GHz帯|54Mbps|  
|IEEE 802.11n|2.4GHz帯および5GHz帯|600Mbps（※１）|  
|IEEE 802.11ac|5GHz帯|6.9Gbps（※２）|  

※１：アンテナを1本のみ搭載している機器の場合、最高リンク速度は約150Mbps  
※２：アンテナを1本のみ搭載している機器の場合、最高リンク速度は約433Mbps  

mjpg-streamerの性能測定の結果、解像度＝1472x736でも80Mbpsの転送速度が必要だが、WiFi接続で、どこまで実効性能が出てくれるか心配・・・  

[新型Raspberry Pi 3 model B+ のスペックと機能の違い](
https://raspida.com/orign-3bplus)  
→ 実効で102Mbps出るみたい

---

#### hostap導入  

送信側を親機（アクセスポイント）、受信側を子機として、直接繋ぐため、「hostap」を導入する  
参考：[無線アクセスポイントとして使用する方法](
http://www.raspberrypirulo.net/entry/2017/02/22/%E7%84%A1%E7%B7%9A%E3%82%A2%E3%82%AF%E3%82%BB%E3%82%B9%E3%83%9D%E3%82%A4%E3%83%B3%E3%83%88%E3%81%A8%E3%81%97%E3%81%A6%E4%BD%BF%E7%94%A8%E3%81%99%E3%82%8B%E6%96%B9%E6%B3%95)  

```
$ sudo apt-get install -y hostapd
$ cd /etc/hostapd/
$ sudo bash -c "zcat /usr/share/doc/hostapd/examples/hostapd.conf.gz > /etc/hostapd/hostapd.conf"
$ sudo vi /etc/hostapd/hostapd.conf
```

hostapd.confの記載内容は、以下を参照  
```
interface=wlan0
driver=nl80211
country_code=JP
channel=36
ssid=XXXXXXXX
hw_mode=a
ignore_broadcast_ssid=0

ieee80211n=1
require_ht=1
ieee80211ac=0
require_vht=1

vht_oper_chwidth=1
vht_oper_centr_freq_seg0_idx=42

ieee80211d=1
ieee80211h=0

wmm_enabled=1
ht_capab=[HT40+][SHORT-GI-40][DSSS_CCK-40]

wpa=2
wpa_passphrase=XXXXXXXX
wpa_key_mgmt=WPA-PSK
rsn_pairwise=CCMP
auth_algs=1
```

※ssidとwpa_passphraseはセキュリティの都合上、上記記載ではXXXXXXXXに  
　自分の環境に合わせて変更必要  
※11ac設定すると、エラーになるので、一旦OFFで運用  

固定するIPをdhcpcd.confに記載  
```
$ sudo vi /etc/dhcpcd.conf
```

以下の内容を最終行に追加  
※ここでは、ラズパイ側のIPアドレス=192.168.250.4としての場合を例示  
```
interface wlan0
static ip_address=192.168.250.4/24
static routers=192.168.250.1
staitc domain_name_servers=192.168.250.1
```

wlan0を、マニュアル設定に
```
$ sudo vi /etc/network/interfaces
```
以下の内容を最終行に追加  
```
allow-hotplug wlan0
iface wlan0 inet manual
```

#### hostapdの起動確認  
```
$ sudo hostapd /etc/hostapd/hostapd.conf
```

設定が正しくできていれば動作開始 → 「wlan0: AP-ENABLED」
```
Configuration file: /etc/hostapd/hostapd.conf
Failed to create interface mon.wlan0: -95 (Operation not supported)
wlan0: interface state UNINITIALIZED->COUNTRY_UPDATE
wlan0: interface state COUNTRY_UPDATE->HT_SCAN
wlan0: Could not connect to kernel driver
Using interface wlan0 with hwaddr b8:27:XX:XX:XX:XX and ssid "XXXXXXXX"
wlan0: interface state HT_SCAN->ENABLED
wlan0: AP-ENABLED
```

※上記では、2行目、5行目にエラー表示が出ているが、問題なく動いている。  
　エラーの意味＆原因は未解決。他のWebサイト見る限り、同様にエラー表示ありで動かしているので、  
　深追いせずに、このまま運用  

ctrl+Cで、起動確認を終了  

#### 自動起動の設定  
下記コマンドでファイルを開く  
```
$ sudo vi /etc/default/hostapd
```

DAEMON_CONFのところにhostapd.confのパスを記入
```
DAEMON_CONF="/etc/hostapd/hostapd.conf"
```

安全のために、起動後にhostapをRestartさせる  
```
$ sudo vi /etc/rc.local
```

"exit 0"の前に、以下の内容を記載
```
service hostapd stop
sleep 5
service hostapd start
```

[前ページ](./raspberry_init.md) / [top](./top.md) / [次ページ](./receiver.md)  
