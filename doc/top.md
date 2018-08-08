# 全天球カメラとVRゴーグルで、テレイグジスタンス体験(1)

## 1. 概略

※[作業一覧](#task_list)へ  

### ■作成するもの  
* 出遅れてるかもしれないが、流行っている「VR」系に乗っかる  
* 全天球カメラで撮影した映像を、別の機材でリアルタイムに視聴する環境  
* 視覚情報だけであるが、テレイグジスタンスを体験できるように  

ステッチされた正距円筒図法のデータを、スフィアに張り付けるようなことはせずに、
撮影画像のDual Fisheye データを地道にリアルタイム処理する。  
和歌山大学 床井研究所の情報を参考にさせていただきました。ありがとうございます。  
[魚眼レンズ画像の平面展開](http://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20160629)  

ちなみに、  
ちょうど、Interface 2018年 7月号で記事になってるみたい。  
<https://shop.cqpub.co.jp/hanbai/books/MIF/MIF201807.html>  

スフィアに張り付ける場合は、以下のサイトなどに書かれている。  
* [THETA Sのmjpegをラズパイで受けてLANでUnityに配信する方法](http://hammmm.hatenablog.com/entry/2016/02/04/211851)  
* [Theta UVC Blenderもどきを作ってみた](https://qiita.com/zinziroge/items/3676b4e0f7715fa60336)  
* [THETA Sの動画をopenframeworksで全天球再生(その２)](http://littlewing.hatenablog.com/entry/2015/12/08/123705)  

---  
### ■用意したもの  
* ラズパイ関連（２セット）  
    * ラズパイ本体  
    →　Raspberry Pi 3 Model B+  
    * microSDカード  
    →　SanDisk microSDHC 98MB/s 32GB Ultra SDSQUAR-032G  
    * 電源ACアダプタ  
    →　Physical Computing Lab 電源セット(5V 5.0A 4USBポート)　Double Cable付き  
    ※ON/OFFスイッチ付きケーブル有
    * ケース（Raspberry Pi 3 Model B+対応のもの）  
    →　RSコンポーネンツ製ケース 白
    * ヒートシンク（念のため）  
    →　waves Raspberry Pi 2/3 Model B 用 ヒートシンク3点  
    * 有線LANケーブル  

* 撮影処理関連  
    * Insta360 Air（MicroUSB版）  
    ※魚眼レンズとセンサーで自作したいところだが、一旦、既製品で代用  
    * フレキシブルケーブル（USB）  
    →　ミヨシ MCO フグ-スネックUSB延長ケ-ブル 30cm USB-EX21BK  
    ※ちょっと硬さが足りないかも・・・ナナメだと、Insta360 Airを支えきれない・・・
    * MicroUSB<->USB変換冶具  
    →　変換名人 USBAA-MCB（USB A オス - microB メス）

* 視点向き処理関連  
    * arduino  
    →　Arduino Uno R3
    * 3DoFセンサー  
    →　Arduino 9軸モーションシールド  
    ※現在、在庫切れ。もしかして製造終了？BOSCH BNO055が使いやすいので、復活希望。
    * スイッチ  
    →　JOYPAD411  
    <https://kata2coro.jimdo.com/joypad411/>  

* 表示処理関連  
    * LCDパネル  
    →　ＳＨＡＲＰ　５．５インチ高精細ＣＧシリコン液晶パネルセット　１０８０×１９２０ドット　ラズパイ用 [LS055-CGS-LCD-SET]  
    * LCDパネルケース  
    →　emerge+ 5.5インチCGシリコン液晶パネルエンクロージャ  
    <https://shop.emergeplus.jp/cgsi-enclosure/>  
    * VRゴーグル  
    →　エレコム 3D VR ゴーグル P-VRG03BK
    * PET透明ボード（1mm厚）
    * M3x12mmネジ（ネジ頭が薄いもの）
    * HDMIケーブル（短め）  
    * USB-MicroUSBケーブル（短め）  

* 機器設置関連  
    * モバイルバッテリー  
    →　cheero Power Plus 10050mAh DANBOARD CHE-066  
    ※USBポートに3.0Aを供給できるものが必要  
    * フィッシングベスト  
    * マジックテープ

* 開発環境
    * WindowsPC  
    * microSDカードリーダー  

---

<a name="task_list"></a>
### ■作業一覧  
* [ラズパイ初期設定](./raspberry_init.md)
* [撮影側ラズパイ構築](./sender.md)  
* [表示側ラズパイ構築](./receiver.md)  
* [FFmpeg導入](./ffmpeg.md)  
* [H264対応](./h264.md)  
* [openFrameworks導入](./openframeworks.md)  
* [全天球からVR表示生成](./vr360.md)  
* [3DoF（arduinoベース）構築](./3dof.md)  
* [VRゴーグル組み上げ](./goggles.md)  
* [結合動作](./complete.md)  
* [微調整](./calibration.md)  

---

### ■結果  

![sender](XXX.jpg)  
![receiver](XXX.jpg)  

とりあえず動いたレベル  
→ソースコード等は、GitHubレポジトリを参照

---

### ■残課題  
* ステッチ調整  
* 魚眼レンズ＋センサーで、全天球カメラモジュールの自作  
* 撮影→表示遅延の削減  
* フレームレートの改善  
* WiFi接続で11ac対応  
* 解像度の向上  
* 3DoF反応速度の向上  
* 前後センサー境界のステッチ処理の最適化  
* 撮影側機器の設置自由化（3軸自動補正）  

