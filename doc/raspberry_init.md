# 全天球カメラとVRゴーグルで、テレイグジスタンス体験(2)

[前ページ](./top.md) / [top](./top.md) / [次ページ](./sender.md)  

## 2. RaspberryPi 初期設定

RaspberryPi購入したら、とりあえず基本的な部分の準備  
ここのサイトがまとまっていて見やすかった。  
* [Raspbianのインストールと最強の初期設定](https://jyn.jp/raspbian-setup/)  

### ■Raspbianの取得
---  
Raspbianを以下のサイトからダウンロード  
<https://www.raspberrypi.org/downloads/raspbian/>  

※デスクトップ画面が必要ない場合、Lite版選択でよい  
　→ ここでは、撮影側はLite版、表示側はDesktop版を選択  
※時期的に、最新で”技適”を通過したばかりの「Raspberry Pi 3 Model B+」を購入したが、  
　新しいものを使うと、そこには茨の道が。。。

### ■WindosPC側の準備
---  
WindowsPC側に、以下のアプリをインストール  
※各アプリの使い方までは、ここでは説明しない。  
* Lhaz : <https://chitora.com/lhaz.html>  
* TeraTerm : <http://ttssh2.osdn.jp/>  
* Filezilla : <https://ja.osdn.net/projects/filezilla/>  
* SDメモリカードフォーマッター Windows用 : <https://www.sdcard.org/jp/downloads/formatter_4/eula_windows/index.html>  
* Win32 Disk Imager : <https://sourceforge.net/projects/win32diskimager/?source=dlp>  
* シリアルデバッグツール 2 : <http://mdrtech.web.fc2.com/upload/upload.htm>  

### ■microSDカードへの書き込み  
---  
WindowsPC上で、以下の操作  
1. 購入したmicroSDを、WindowsPCに刺し、SDメモリカードフォーマッターでフォーマット  
2. Raspbianのサイトからダウンロードしてきたファイルを解凍  
3. 解凍してできたファイルを、Win32 Disk ImagerでmicroSDに書き込み  
    書き込みが終わったら、エクスプローラ上で２つのドライブレターが新規に見えている状態になる  
        「boot」と「USB ドライブ」  
    ※「USB ドライブ」は、Widowsからアクセスできない領域。”使うにはフォーマットが必要”とのメッセージが出るが、キャンセルする。  

4. 外部からラズパイに対し、SSH経由で接続するため、  
    「boot」側のドライブに、拡張子無しの 「ssh」 ファイルを作成（ファイルの中身は空でいい）  

    ※新規作成では、txt拡張子のファイルができてしまう。  
    　エクスプローラで、拡張子の表示をOFFにしている場合、拡張子がついていることに気づけないので要注意。  

上記が終わったら、microSDをWindowsPCから抜き、ラズパイに挿入

### ■ラズパイの初回起動  
---  
1. ラズパイに、有線LANケーブルを接続し、電源ON  
    ※作業環境の有線LANネットワークに、ルータ等があり、DHCPにてIPアドレスが割り当てられる前提

2. ルータ側の設定画面で、ラズパイに割り当てたられたIPアドレスを確認

    ※参考：WindowsPCから調べる手段  
    [arpコマンドを使って、IPアドレスを調べる](https://qiita.com/xshell/items/af4e2ef8d804cd29e38e)  

3. WindowsPCのTeraTermより、SSHでラズパイに接続  
    初期ユーザー名：pi  
    初期パスワード：raspberry  

4. まずは、Raspbianを最新に更新  
    ```  
    $ sudo apt-get update && sudo apt-get -y dist-upgrade && sudo apt-get -y autoremove && sudo apt-get autoclean
    $ sudo apt-get install rpi-update
    $ sudo rpi-update
    ```  

5. raspi-config設定  
```  
$ sudo raspi-config
```  

    * [8 Update]  
        更新実行  
    * [4 Localisation Options] → [I2 Change Timezone]  
        "Asia->Tokyo"を選択  
    * [4 Localisation Options] → [I4 Change Wi-fi Country]  
        JP Japan"を選択  
    * その他の設定補足  
        * microSDの容量拡張：不要  
        * sshの有効化：不要  
        * serialとBluetoothは、排他利用なので注意  
        * RaspbianLite版は、GUIなし  
        * OpenGLは、ES版のままで  
        * GPUメモリ割り当ては、後で変更  
        * Wifi country設定、後で変更  
        * ロケールは、そのまま  
        * キーボード設定は、自分の使用するものに合わせて変更  
        * Overclockは、このメニューでは設定できなくなったらしい  
        * 上記以外の設定は、こちらを参照  
        http://igarashi-systems.com/sample/translation/raspberry-pi/configuration/raspi-config.html  

### ■ラズパイの各種設定
---  
1. 自分の好みに合わせて各種作業環境を整備  
    ```  
    sudo apt-get install -y vim
    sudo apt-get install -y autopoint pkg-config libtool build-essential autoconf automake
    sudo apt-get install -y git cmake
    ```  

    * vimでマウスを無効にする  
    下記実行後、"set mouse"の設定をコメントアウトして保存  
    ```
    sudo vi /usr/share/vim/vim80/defaults.vim
    ```

2. 時刻設定  

    ntpdateのインストール  
    インストールが終わったらNTPサーバーを指定して時刻合わせを行う。  
    ※ここでは、ＮＩＣＴ公開ＮＴＰサービスを使用  
    ```  
    $ sudo apt-get install -y ntpdate
    $ sudo ntpdate -v ntp.nict.jp
    $ date
    ```  

    ntpのインストール  
    ```  
    $ sudo apt-get -y install ntp
    $ sudo vi /etc/ntp.conf
    ```  

    以下の設定を追記（それ以外のpool設定は、コメントアウト）  
    ```  
    pool ntp.nict.jp iburst  
    ```  

    NTPサービスを再起動  
    ```  
    $ sudo service ntp restart
    ```  

### ■モニターの接続設定  
---  
<http://sc.akizukidenshi.com/catalog/g/gM-11967/?customer=z>  
秋月電子で売っている”Sharp 5.5inch CGシリコン液晶パネルセット [LS055-CGS-LCD-SET]”を、HDMIモニターとして使うので、起動設定を行う。  

config.txtに、モニターの接続設定を追加する
```  
$ sudo vi /boot/config.txt
```  

秋月電子のWebサイトに記載されている`config.txt`は、バグがあるので要注意  
修正点は以下の３つ  
* 日本語コメントの削除
* hotplug設定の誤植を修正
* 縦横回転に柔軟に対応できるように、バッファサイズの縦横設定を拡張

※追加部分のみ記載  
```  
#==================================================================================#
# HDMI output setting for SHARP LS055T1SX01A
#==================================================================================#
# HDMI Basic configuration
#==================================================================================#
hdmi_pixel_freq_limit=400000000
hdmi_drive=2
disable_overscan=1
dtparam=audio=on
start_x=1
hdmi_group=2
hdmi_mode=87
hdmi_force_hotplug=1
gpu_mem=128
dtparam=spi=on
dtparam=i2c_arm=on
#==================================================================================#
# 1080 x 1920 5.5Inch
#==================================================================================#
# Main Setting
#----------------------------------------------------------------------------------#
hdmi_timings=1080 0 60 10 35 1920 0 4 4 2 0 0 0 50 0 114352500 0
max_framebuffer_width=1920
max_framebuffer_height=1920
#----------------------------------------------------------------------------------#
# Portrait or Landscape Setting
#Portrait-1 (FPC is bottom side.)
#----------------------------------------------------------------------------------#
#display_rotate=0
#framebuffer_width=1080
#framebuffer_height=1920
#----------------------------------------------------------------------------------#
# Landscape-1 (FPC is right side.)
#display_rotate=1
#----------------------------------------------------------------------------------#
display_rotate=1
framebuffer_width=1920
framebuffer_height=1080
#----------------------------------------------------------------------------------#
# Portrait-2 (FPC cable is upper side.)
#display_rotate=2
#----------------------------------------------------------------------------------#
#display_rotate=2
#framebuffer_width=1080
#framebuffer_height=1920
#----------------------------------------------------------------------------------#
# Landscape-2 (FPC is left side.)
#display_rotate=3
#----------------------------------------------------------------------------------#
#display_rotate=3
#framebuffer_width=1920
#framebuffer_height=1080
#----------------------------------------------------------------------------------#
```  

#### ＜モニター使用時の注意点＞  
* 購入後、初回電源ON後に、基板のセットアップが必要  
    ※参考：<http://sc.akizukidenshi.com/download/ds/sharp/LS055-CGS-LCD-SET_manual_i.pdf>  
* 電源の供給は、ラズパイのUSBポートからでも可能  
    ※ラズパイ自体に供給する最大電流の範囲を越えない場合  
* モニターの起動処理は、ラズパイの起動処理より遅いので、表示されるまでしばらく待つ必要がある  

設定を書き込んだら、モニターを接続して再起動  
```  
$ sudo reboot
```  
正しく表示されることを確認  
※ちなみに、リブートしたら、WindowsPC側のTeraTermがクローズされる。  
　ラズパイの起動処理が終わるまで数秒ほど待ってから、再度接続しなおす。  

### ■設定内容の保存
---  
上記設定が終わったら、ラズパイをシャットダウンして、microSDを取り出す。  
```  
$ sudo shutdown -h now
```  

WindowsPC上で、以下の操作  
* microSDを、WindowsPCに刺し、Win32 Disk ImagerでmicroSDから読み出し  
※容量が拡張されているので、読み出しに時間かかる  
　イメージを最小限にしてバックアップする場合は、以下を参照  
　[raspberry piのイメージファイルを小さな容量のメモリーカードにコピーする方法](https://qiita.com/NaotakaSaito/items/b7ab1407b171eaa459c5)  

---
※映像送信側、映像受信側の2台分の環境をセットアップする  

[前ページ](./top.md) / [top](./top.md) / [次ページ](./sender.md)  
