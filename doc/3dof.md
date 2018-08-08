# 全天球カメラとVRゴーグルで、テレイグジスタンス体験(9)

[前ページ](./vr360.md) / [top](./top.md) / [次ページ](./goggles.md)  

## 9. 3DoF（arduinoベース）構築  

VRゴーグルを装着する頭部の向きを、リアルタイムに検出する。  
センサーやスイッチを接続するにあたって、ラズパイに直接接続することも可能であるが、ソフト/ハード両面で実装を簡易化するため、arduinoを使う。  

### ■arduino  
---
ラズパイとの接続はUSBで行う。USBで、電力供給とシリアル通信を兼ねる。  
ラズパイ側からは、`/dev/ttyACM0`として見える。

※補足  
* arduinoのソースコードの拡張子が`.ino`なので、自分の使うエディタにC言語用として登録しておく  
* 処理を記載する主要な関数は２つ：`setup()`、`loop()`  
* Arduino IDEで、メニュー「ツール」→「ボード」で、対象のボードを適切に選択  
* Arduino IDEで、メニュー「スケッチ」→「ライブラリをインクルード」で、必要なライブラリは事前に取り込んでおく  
* firmataは、今回使用なし  

### ■BOSCH BNO055  
---
Arduino 9軸モーションシールドには、BOSCH BNO055が搭載されている。
物理的にはこのシールドを、Arduino UNO R3にピッタリはめるだけ。  
I2C接続なので、使うピンはA4とA5  

arduinoでのBNO055の使い方は、以下のサイトの情報を参考にさせていただきました。  
ありがとうございます。  
[Devel/電子工作/Arduino/モーションセンサ/BNO055](https://cubic9.com/Devel/)

※読む必要ないですが、一応、BNO055のデータシートは以下  
　開発途中に課題発生し、紆余曲折してがっつり読むことになったが、結局別のバグ修正で解決  
<https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST_BNO055_DS000_14.pdf>  

3軸加速度センサ、ジャイロ（角速度）センサ、3軸地磁気センサの値が取得できるので、これで頭部の方向を決めれる。しかし、計算は大変らしい。  
ありがたいことに、  
BNO055自体で、上記センサ値よりクォータニオンを算出してくれるので、その値を使う。  

さらにありがたいことに、
BNO055をレジスタレベルで直接制御せずに、関数化したものが用意されているので、そちらも使用する。

1. 「Adafruit_BNO055 Library Adafruit」及び「Adafruit_Sensor Library	Adafruit」を、以下のサイトからダウンロード  
    <https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/arduino-code>  
2. 上記でダウンロードした2ファイルを解凍し、Arduino用フォルダのlibrariesにコピーする。もしくは、「ライブラリをインクルード」する。
3. Arduino IDEで、メニュー「ファイル」→「スケッチの例」→「Adafruit BNO055」→「rawdata」を選択で、サンプルコードを表示

※上記ライブラリとfirmataを合体させようとしたのだが、うまくいかず  

#### クォータニオン

まだちゃんと理解できていないので、ここではクォータニオン自体の解説は省略  
調べれば、いろいろとヒットするので、他の方の解説を参照で  

クォータニオンの理解は、は、以下のサイトを参照させていただいた  
* [組込み技術者のための四元数（クォータニオン）入門（１）：基礎編](https://kumadasu.com/2017/09/20/%E7%B5%84%E8%BE%BC%E3%81%BF%E6%8A%80%E8%A1%93%E8%80%85%E3%81%AE%E3%81%9F%E3%82%81%E3%81%AE%E5%9B%9B%E5%85%83%E6%95%B0%EF%BC%88%E3%82%AF%E3%82%A9%E3%83%BC%E3%82%BF%E3%83%8B%E3%82%AA%E3%83%B3%EF%BC%89/)  
* [クォータニオン（回転）についてメモ](http://edom18.hateblo.jp/entry/2018/06/25/084023)  
* [クォータニオンと回転](https://www.f-sp.com/entry/2017/06/30/221124)  


BNO055のライブラリとしては、起動時に`begin()`関数の引数に、OPERATION_MODE_IMUPLUSを設定  
`getQuat()`関数で、クォータニオンが取得できる。更新の粒度は、100Hz  

### ■JOYPAD411  
---

本構成のユーザーインターフェイスとして、スイッチを追加する。  
目的は、２つ  
* 頭部方向の初期値を決めるため、クォータニオン算出のリセット  
* 撮影側カメラの設置方向に対する手動調整（本来は、自動にすべき）  

ちょうど、十字入力できるスイッチがついているので、この部品（JOYPAD411）を使う。  

|スイッチ|内容|  
|:---|:---|  
|赤スイッチ|クォータニオンリセット|  
|黄スイッチ|モード変更（通常 → Yaw/Pitch調整 → Roll調整 → 調整クリア → 通常）|  
|緑スイッチ（通常時）|無反応|  
|緑スイッチ（Yaw/Pitch調整時）|上下押しでPitch調整、左右押しでYaw調整|  
|緑スイッチ（Roll調整時）|左右押しでRoll調整|  
|緑スイッチ（調整クリア時）|下押しでクリア|  

arduinoとの接続は、以下の通り  

|ピン|スイッチ|  
|:---|:---|  
|D2|黄色|  
|D4|赤色|  
|D7|緑色（右）|  
|D8|緑色（下）|  
|D12|緑色（左）|  
|D13|緑色（上）|  

※すべて内部PULL_UP設定で接続  

緑スイッチで、Yaw/Pitch/Rollを調整する。  
openFrameworks側は、以下のサイトの情報を活用  
* [回転 Quaternion その１ ｜3Dプログラミング整理](http://studio23c.hatenadiary.jp/entry/%3Fp%3D664)  

### ■シリアル通信仕様  
---

ボーレート＝115200bps  
→1秒間に14400文字（1文字＝8Byte換算）  
→60fpsであれば240文字送れる。  

#### シリアル通信プロトコル  
”独自実装しました”的なサイトがよくヒットするのだが、既にあるものを使う方向で  
* COBS：[Consistent Overhead Byte Stuffing](https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing)
* SLIP：[Serial Line Internet Protocol](https://en.wikipedia.org/wiki/Serial_Line_Internet_Protocol)

arduino用は、以下のコードを使う  
PacketSerial  
https://github.com/bakercp/PacketSerial  

COBSとSLIP、どっちが主流かよくわからない。今回は、RFCにもなっているようなのでSLIPを使う。  
Arduino用フォルダのlibrariesに、PacketSerialフォルダを作成し、src以下のファイルをすべてコピー  

```
＜データ形式＞
固定長21文字=ヘッダー5文字＋クオータニオン値（w、x、y、z）＋スイッチ（赤、黄、左、下、上、右）＋フッター1文字＋改行  
```

※シリアル受信側（ラズパイ）で、正しく受信できなければ値を使わなければいいだけなので、一時的なデータ化けに対するリカバリー処理はしない。  

[前ページ](./vr360.md) / [top](./top.md) / [次ページ](./goggles.md)  
