<<<<<<< HEAD
# Multipath-TCP in ns-3 on Docker
This repository can build the environment of [mkheirkhah/mptcp](https://github.com/mkheirkhah/mptcp) on docker.  

# How to use
srcディレクトリに移動後、[mkheirkhah/mptcp](https://github.com/mkheirkhah/mptcp)をcloneする。mptcpというディレクトリが作成されている。
その後、元のディレクトリに戻ったらdockerを立ち上げる。
なお、srcディレクトリはホストコンピュータとコンテナで共有されている。

※ Mac/Windowsの場合、Docker-Desktopを起動した状態でないと、`docker-compose` コマンドを利用することができないので立ち上げる。
```

ここは自分で書いたところ
,,,
home/outlook/MPTCP-Dockerでdocker-compose up -dとdocker-compose exec mptcp /bin/bashをやる
（Docker Desktopのns3-dockerを起動させておく必要がある）
,,,

$ cd src 
# git clone https://github.com/mkheirkhah/mptcp.git
$ cd ../
$ docker-compose up -d
```

コンテナを立ち上げた後、コンテナの中に入る。
その後、cloneしたディレクトリに入り、コンパイルする。
最後の行はscratchディレクトリ内のmptcpファイルを実行している。
このmptcpファイルや、exampleディレクトリにあるファイルを参考にシナリオを書いてみてください。
```
$ docker-compose exec mptcp /bin/bash
$ cd /src/mptcpls
$ ./waf configure
$ ./waf --run "mptcp"
```

なお、MPTCPに関する実装は
```
src/mptcp/src/internet/model/mp-tcp-socket-base.cc
```
等にあり、パケットスケジュールアルゴリズムなどはここに実装されていました。
パケットスケジュールアルゴリズムの改良の研究の場合（僕の研究がそうでしたが）このファイルを改変していました。
>>>>>>> Initial commit
