# 全天球カメラとVRゴーグルで、テレイグジスタンス体験(4)

[前ページ](./sender.md) / [top](./top.md) / [次ページ](./ffmpeg.md)  

## 4. 表示側のラズパイ環境構築

撮影側ラズパイからネットワーク経由で映像データを取得し、再生＆表示する。  

### ■MotionJPEG再生確認（有線LAN）
---
撮影側ラズパイで、mjpg_streamerを起動させておく  
表示側ラズパイに、5.5inchモニターを接続  

表示側ラズパイで、以下のコマンドを実行  
```
$ sudo apt-get install -y omxplayer
$ omxplayer --win '0 0 1920 1080' 'http://192.168.11.113:8080/?action=stream'
```
※OMXPlayerは、ラズパイ専用のハード支援機能を使ってメディア再生するアプリ  
※リモート接続のTeraTerm側コンソールで実行しても、表示はモニター側に出力されるので注意  

※ロジクールのC270で、MotionJPEGのまま配信した場合、  
　WindowsPCでは再生できたが、OMXPlayerでは再生できなかった。  
　C270が生成するMotionJPEGデータのフォーマットで、どこか課題があるのかも・・・  

#### 解像度＝1472x736の場合  

＜CPU性能:受信側＞  
```  
Linux 4.14.52-v7+ (raspberrypi)         08/02/18        _armv7l_        (4 CPU)

09:38:49     CPU    %usr   %nice    %sys %iowait    %irq   %soft  %steal  %guest  %gnice   %idle
09:38:49     all    0.52    0.00    1.37    0.14    0.00    0.19    0.00    0.00    0.00   97.77
09:38:49       0    0.49    0.00    1.53    0.28    0.00    0.64    0.00    0.00    0.00   97.06
09:38:49       1    0.49    0.00    1.38    0.09    0.00    0.06    0.00    0.00    0.00   97.98
09:38:49       2    0.49    0.00    1.17    0.13    0.00    0.04    0.00    0.00    0.00   98.16
09:38:49       3    0.62    0.00    1.42    0.08    0.00    0.03    0.00    0.00    0.00   97.85
```  

＜ネットワークトラフィック:受信側＞  
```  
   # Host name (port/service if enabled)            last 2s   last 10s   last 40s cumulative
--------------------------------------------------------------------------------------------
   1 192.168.11.66                            =>     35.1Kb     65.2Kb     65.2Kb     81.5KB
     raspberrypi                              <=     56.0Mb     54.6Mb     54.6Mb     68.2MB
   2 192.168.11.66                            =>     7.69Kb     5.99Kb     5.99Kb     7.49KB
     DESKTOP-SISINN                           <=       320b       256b       256b       320B
   3 192.168.11.66                            =>         0b       230b       230b       288B
     router.asus.com                          <=         0b       343b       343b       429B
--------------------------------------------------------------------------------------------
Total send rate:                                     42.8Kb     71.4Kb     71.4Kb
Total receive rate:                                  56.0Mb     54.6Mb     54.6Mb
Total send and receive rate:                         56.0Mb     54.7Mb     54.7Mb
--------------------------------------------------------------------------------------------
Peak rate (sent/received/total):                      155Kb     59.1Mb     59.3Mb
Cumulative (sent/received/total):                    89.2KB     68.2MB     68.3MB
============================================================================================
```  

OMXPlayerで、MotionJPEG時にハード支援機能が有効かどうか？の情報を、見つけることができず。CPU性能の計測結果を見る限りは、有効そうにみえる。  
しかし、ネットワークトラフィックの方が問題で、  
有線LANの性能として300Mbpsまでいけるはずだが、75.6Mb必要なのに対し54.6Mbしか出ていない。  
また、別の原因かもしれないが、表示としても、目視レベルで2秒ぐらい遅延している。。。  

性能改善は、一旦、後回しにして、構築作業を進める。  

### ■WiFi設定（子機）  
---

固定するIPをdhcpcd.confに記載  
```
$ sudo vi /etc/dhcpcd.conf
```

以下の内容を最終行に追加  
※ここでは、ラズパイ側のIPアドレス=192.168.250.3としての場合を例示  
```
interface wlan0
static ip_address=192.168.250.3/24
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
wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf
```

親機側で設定したSSIDとPASSPHRASEを、子機に設定  
※`hostapd.con`で設定した値を入力
```
$ sudo sh -c 'wpa_passphrase SSID PASSPHRASE >> /etc/wpa_supplicant/wpa_supplicant.conf'
```

/etc/wpa_supplicant/wpa_supplicant.confファイル内では、  
生パスワードがコメントアウトで記載されてしまうので、確認してから削除する  
```
$ sudo vi /etc/wpa_supplicant/wpa_supplicant.conf
```

再起動後、親機と接続できているかをpingで確認  
```
$ ping 192.168.250.4
```

### ■MotionJPEG再生確認（WiFi）
---
有線LANの時と同様に、表示側ラズパイで、以下のコマンドを実行  
```
$ omxplayer --win '0 0 1920 1080' 'http://192.168.250.4:8080/?action=stream'
```

#### 解像度＝1472x736の場合  

＜CPU性能:受信側＞  
```  
Linux 4.14.58-v7+ (raspberrypi)         02/08/18        _armv7l_        (4 CPU)

15:40:45     CPU    %usr   %nice    %sys %iowait    %irq   %soft  %steal  %guest  %gnice   %idle
15:40:45     all    0.97    0.00    0.99    0.26    0.00    0.12    0.00    0.00    0.00   97.66
15:40:45       0    1.63    0.00    1.08    0.38    0.00    0.28    0.00    0.00    0.00   96.63
15:40:45       1    0.70    0.00    0.65    0.23    0.00    0.02    0.00    0.00    0.00   98.41
15:40:45       2    0.85    0.00    1.34    0.24    0.00    0.13    0.00    0.00    0.00   97.44
15:40:45       3    0.74    0.00    0.88    0.19    0.00    0.08    0.00    0.00    0.00   98.12
```  

＜ネットワークトラフィック:受信側＞  
```  
   # Host name (port/service if enabled)            last 2s   last 10s   last 40s cumulative
--------------------------------------------------------------------------------------------
   1 192.168.250.3                            =>     26.4Kb     23.7Kb     23.7Kb     29.7KB
     192.168.250.4                            <=     52.0Mb     47.6Mb     47.6Mb     59.5MB
--------------------------------------------------------------------------------------------
Total send rate:                                     26.4Kb     23.7Kb     23.7Kb
Total receive rate:                                  52.0Mb     47.6Mb     47.6Mb
Total send and receive rate:                         52.0Mb     47.7Mb     47.7Mb
--------------------------------------------------------------------------------------------
Peak rate (sent/received/total):                     26.4Kb     52.0Mb     52.0Mb
Cumulative (sent/received/total):                    29.7KB     59.5MB     59.6MB
============================================================================================
```  

ネットワークトラフィックは、有線LANとほぼ同じ。  

### ■ネットワーク自体の性能測定  
---
[ネットワーク測定ツールiperfの使い方](https://qiita.com/takish/items/bff7a1df712d475432df)  

送信側、受信側の両方にiperfをインストール  
```
$ sudo apt-get install -y iperf
```

#### 有線LAN性能
送信側(IPアドレス例＝192.168.11.113)  
```
$ iperf -s
```

受信側(IPアドレス例＝192.168.11.66)  
```
$ iperf -c 192.168.11.113
```

結果  
```
------------------------------------------------------------
Server listening on TCP port 5001
TCP window size: 85.3 KByte (default)
------------------------------------------------------------
[  4] local 192.168.11.113 port 5001 connected with 192.168.11.66 port 38732
[ ID] Interval       Transfer     Bandwidth
[  4]  0.0-10.1 sec   220 MBytes   183 Mbits/sec
```
```
------------------------------------------------------------
Client connecting to 192.168.11.113, TCP port 5001
TCP window size: 43.8 KByte (default)
------------------------------------------------------------
[  3] local 192.168.11.66 port 38732 connected with 192.168.11.113 port 5001
[ ID] Interval       Transfer     Bandwidth
[  3]  0.0-10.0 sec   220 MBytes   184 Mbits/sec
```

#### WiFi性能
送信側(IPアドレス例＝192.168.250.4)  
```
$ iperf -s
```

受信側(IPアドレス例＝192.168.250.3)  
```
$ iperf -c 192.168.250.4
```

結果  
```
------------------------------------------------------------
Server listening on TCP port 5001
TCP window size: 85.3 KByte (default)
------------------------------------------------------------
[  4] local 192.168.250.4 port 5001 connected with 192.168.250.3 port 35064
[ ID] Interval       Transfer     Bandwidth
[  4]  0.0-10.1 sec  92.2 MBytes  76.8 Mbits/sec
```
```
------------------------------------------------------------
Client connecting to 192.168.250.4, TCP port 5001
TCP window size: 43.8 KByte (default)
------------------------------------------------------------
[  3] local 192.168.250.3 port 35064 connected with 192.168.250.4 port 5001
[ ID] Interval       Transfer     Bandwidth
[  3]  0.0-10.0 sec  92.2 MBytes  77.3 Mbits/sec
```

---
有線LANの性能は問題なし。  
WiFiの性能は、解像度＝1472x736のMotionJPEGを送るのは、ギリギリなので無理そう。。。  

[前ページ](./sender.md) / [top](./top.md) / [次ページ](./ffmpeg.md)  

