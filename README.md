# ThinkerV3
## 概要
["AlphaZero 深層学習・強化学習・探索 人工知能プログラミング実践入門"(株式会社ボーンデジタル)](https://www.amazon.co.jp/AlphaZero-%E6%B7%B1%E5%B1%A4%E5%AD%A6%E7%BF%92%E3%83%BB%E5%BC%B7%E5%8C%96%E5%AD%A6%E7%BF%92%E3%83%BB%E6%8E%A2%E7%B4%A2-%E4%BA%BA%E5%B7%A5%E7%9F%A5%E8%83%BD%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0%E5%AE%9F%E8%B7%B5%E5%85%A5%E9%96%80-%E5%B8%83%E7%95%99%E5%B7%9D-%E8%8B%B1%E4%B8%80/dp/4862464505?__mk_ja_JP=%E3%82%AB%E3%82%BF%E3%82%AB%E3%83%8A&crid=23HZL4ACT6NSF&dib=eyJ2IjoiMSJ9.9y8dkCsxh6_b-E-08N8xpeDeVLz_V2TNs9JjN531QtZbtkYoejpXi39tF0i-FZlYv08LWfnnQl35z7IQWIGHrxp8oko4wszBsUQu3oEgCpGFy-q0FTrPxGndzIm7Q9lIRPLJV-NVNfud-pIoj4U75WIBuRMAcGofA00xKI9J4w_wC1hXeVf8oFVj-m2yCbtOSOHYSsZsw4sfOL6-5sJtCkoCD0p8kI23l4qKcjuBpN_lvm15k56BxH5rVhW-lmmI65a5fzHxOrBov1B2ai9zEGS_4D2L0HX5_p7zAtHvJlqf0-wO9QY9Ys6ZgDYgBy4a.kumXFWo33VfDXqxrPTODo8r8vfv143HWsXDT6t4x_-4&dib_tag=se&keywords=AlphaZero+%E6%B7%B1%E5%B1%A4%E5%AD%A6%E7%BF%92%E3%83%BB%E5%BC%B7%E5%8C%96%E5%AD%A6%E7%BF%92%E3%83%BB%E6%8E%A2%E7%B4%A2+%E4%BA%BA%E5%B7%A5%E7%9F%A5%E8%83%BD%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0%E5%AE%9F%E8%B7%B5%E5%85%A5%E9%96%80&qid=1741977020&sprefix=alphazero+%E6%B7%B1%E5%B1%A4%E5%AD%A6%E7%BF%92+%E5%BC%B7%E5%8C%96%E5%AD%A6%E7%BF%92+%E6%8E%A2%E7%B4%A2+%E4%BA%BA%E5%B7%A5%E7%9F%A5%E8%83%BD%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0%E5%AE%9F%E8%B7%B5%E5%85%A5%E9%96%80%2Caps%2C175&sr=8-1&linkCode=ll1&tag=sasihara-22&linkId=790dfcb949f1ba93d358633f6dfb1682&language=ja_JP&ref_=as_li_ss_tl)を参考に、C++言語で実装したDeep Learningベースの思考ルーチンです。

## 制限事項
週末の趣味程度で開発しているものなので、完成度は期待しないで下さい。

また一緒に格納してある学習モデルは1000程度の対局データしか学習しておらず、負けデータばかりの偏ったデータなことから、かなり弱いです(min-maxベースの内蔵思考ルーチンに対して勝率0.2%程度のレベル)。04_OthelloDeepModelレポジトリから最新版のモデルを上書きコピーして下さい。
## 推奨環境
Intel Core i5 2.4GHz程度のCPUであれば十分遊べますが、GPUがあると思考時間がより短くなります。
## 開発環境
Microsoft Visual Studio Community 2022での動作を確認しております。それ以外の環境については未確認ですが、基本的なAPIしか使用してないため、他のバージョンでも動作する可能性は高いと思います。
## コンパイル・実行
- 本ソースのコンパイルには、レポジトリ"01_othello"に含まれるソースも必要ですので、レポジトリ"01_othello"もダウンロードしておいて下さい。
- プロジェクトファイルはthinkerV3.slnです。Visual Studioでオープンしコンパイルすると、実行バイナリがx64\Releaseフォルダの下に作成されますので、それらをダブルクリックすることで実行できます。

- 本思考ルーチンの実行には、tensorflow.dllが必要となります。[TensorFlow for C のインストール](https://www.tensorflow.org/install/lang_c?hl=ja)のページからダウンロードしてください。
  - "セットアップ"節の下の"ダウンロード"の下にある"Windows、CPU のみ"もしくは"Windows、GPU のみ"のlibtensorflow-XXX-windows-x86_64-2.6.0.zipをダウンロードして解凍してください。解凍後、libtensorflow-cpu-windows-x86_64-2.6.0フォルダ一式を03_thinkerV3フォルダと同じフォルダに格納して下さい。また、その中に含まれるtensorflow.dllをthinkerV3.exeと同じフォルダに格納してください。
  - 上記tensorflow.dll 2.6.0のGPU版を動かすためには、さらに[CUDA](https://developer.nvidia.com/cuda-toolkit-archive)、[cuDNN](https://developer.nvidia.com/rdp/cudnn-archive)、zlibが必要となります。tensorflow.dll 2.6.0自体古いため、CUDA、cuDNNについては本ライブラリがリリースされた当時のバージョンを入れる必要があります。2022年8月ごろにリリースされたバージョン([CUDA 11.7.1](https://developer.nvidia.com/cuda-11-7-1-download-archive), [cuDNN 8.5.0 (August 8th, 2022) for CUDA 11.X](https://developer.nvidia.com/rdp/cudnn-archive))で動作することを確認しております。
- thnkerV3.exeを実行すると、デフォルトではUDPポート番号60001でメッセージ待ち受け状態に入ります。"01_othello"プロジェクトに格納されるothello.exeを実行し、
ボード上をクリックすることで表示されるゲームの初期設定画面において、"Computer(External)"にチェックを入れた後、黒もしくは白のHost Nameに"localhost"を、Portに"60001"をセットすることで、本思考ルーチンを用いてプレーすることができます。

## 開発履歴
### 2025/11/9(V3.20)
強さに直接影響を与えるモンテカルロ法処理部分の価値計算におけるバグを修正｡
### 2025/5/10
終盤はDeep Learningベースではなく、完全読みを行うように修正。
### 2025/3/9
初期バージョン。

## NOTICE
This software includes the work that is distributed in the Apache License 2.0. <BR>
このソフトウェアは、 Apache 2.0ライセンスで配布されている製作物が含まれています。
