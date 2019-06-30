# my HTTP/1.1
練習のための簡単なHTTPサーバ

# 実装済みの機能
- GETリクエストに対する応答
  - html,画像 (png, jpeg) に対応
- それ以外のリクエストには501を返す

# 使い方
1. コンパイル  
```gcc serv.c -o serv```  
```gcc serv.c -o serv -DDEBUG``` とすると受け取ったリクエストが標準エラー出力に出力されるようになる  

1. サーバの実行  
```./serv```  
localhostの12345番ポートでリクエストを待ち受ける
	- 終了は`ctrl+c`

1. ブラウザからローカルのhtmlファイルにアクセスする  
例: `http://localhost:12345/index.html`  
(サーバと同じフォルダにindex.htmlがある場合)

# 参考
RFC2616 (https://tools.ietf.org/html/rfc2616) など
