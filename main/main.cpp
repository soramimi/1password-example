
// 標準入出力・文字列操作・環境変数取得に使用する標準ヘッダ
#include <stdio.h>
#include <string.h>
// プロジェクト内のローカルヘッダ
#include "main.h"
// GoのCGOで生成された共有ライブラリ libapikey.so のヘッダ
// GetAPIKEY() および FreeString() の宣言が含まれる
#include "../libapikey/libapikey.h"

/**
 * メインエントリポイント
 *
 * 1Password Desktop App Integration を使ってシークレット（APIキー等）を取得し、
 * 標準出力に表示する。
 *
 * 前提条件:
 *   - 1Password デスクトップアプリがインストール・サインイン済みであること
 *   - 環境変数 OP_ACCOUNT_NAME にアカウント名（表示名）が設定されていること
 */
int main(int argc, char **argv)
{
	// 環境変数 OP_ACCOUNT_NAME から 1Password のアカウント名を取得する
	// 例: OP_ACCOUNT_NAME="John Doe" ./myapp
	char *account_name = getenv("OP_ACCOUNT_NAME");
	if (!account_name) {
		// 環境変数が設定されていない場合はエラーを出力して終了する
		fprintf(stderr, "OP_ACCOUNT_NAME is not specified\n");
		exit(1);
	}

	// libapikey.so 経由で 1Password SDK を呼び出し、シークレットを取得する
	// 戻り値はGoのランタイムが確保したヒープ上の文字列ポインタ
	char *s = GetAPIKEY(account_name);

	// 取得したシークレットを標準出力に表示する
	puts(s);

	// セキュリティ上の理由から、メモリに残ったシークレットをゼロクリアする
	// これにより、プロセス終了後にメモリダンプ等でシークレットが漏洩するリスクを低減する
	memset(s, 0, strlen(s));

	// Goのランタイムが確保したメモリを解放する
	// C側で free() を直接呼ぶと問題が起きるため、必ず FreeString() を使う
	FreeString(s);
	return 0;
}
