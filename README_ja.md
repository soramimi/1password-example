# 1password-example

1Password Desktop App Integration を使って、C/C++ アプリケーションからシークレット（APIキー等）を安全に取得するサンプルプロジェクトです。

## 概要

1Password の公式 Go SDK を CGo で共有ライブラリ化し、C++ から呼び出すという構成を採用しています。シークレットはコードやファイルにハードコードせず、1Password Vault から実行時に取得します。

```
┌─────────────────────────────────────────────────────┐
│  myapp (C++)                                        │
│    ↓ GetAPIKEY() を呼び出す                          │
│  libapikey.so (Go + CGo)                           │
│    ↓ 1Password SDK でシークレットを解決              │
│  1Password デスクトップアプリ                        │
│    ↓                                               │
│  1Password Vault (op://API_KEY/Example/credential) │
└─────────────────────────────────────────────────────┘
```

## ディレクトリ構成

```
1password-example/
├── Makefile              # プロジェクト全体のビルド・実行スクリプト
├── libapikey/            # Go 製の共有ライブラリ
│   ├── main.go           # GetAPIKEY / FreeString の実装
│   ├── libapikey.h       # CGo が生成するエクスポートヘッダ
│   ├── go.mod            # Go モジュール定義
│   └── Makefile          # libapikey.so のビルドスクリプト
└── main/                 # C++ 製のメインアプリケーション
    ├── main.cpp          # エントリポイント
    ├── main.h            # ローカルヘッダ
    └── Makefile          # myapp のビルドスクリプト
```

## 前提条件

- [1Password デスクトップアプリ](https://1password.com/downloads/) がインストール・サインイン済みであること
- 1Password Vault に `API_KEY` という名前の Vault があり、`Example` アイテムの `credential` フィールドにシークレットが登録されていること
- Go 1.26.4 以降
- GCC / G++
- Make

## ビルド方法

```bash
make
```

内部では以下の順序でビルドされます。

1. `libapikey/` で Go の共有ライブラリ (`libapikey.so`) をビルド
2. `main/` で C++ アプリ (`myapp`) をビルドし、`libapikey.so` をリンク

## 実行方法

```bash
OP_ACCOUNT_NAME="アカウント表示名" make run
```

環境変数 `OP_ACCOUNT_NAME` には、1Password に登録されているアカウントの表示名（例: `John Doe`）を指定します。

実行すると、1Password Vault から取得したシークレットが標準出力に表示されます。

## API リファレンス

### `GetAPIKEY(account_name *C.char, secret_ref_uri *C.char) *C.char`

1Password Desktop App Integration 経由でシークレットを取得します。

| 項目 | 説明 |
|------|------|
| 第1引数 | `account_name` — 1Password アカウントの表示名 |
| 第2引数 | `secret_ref_uri` — シークレット参照 URI（例: `op://API_KEY/Example/credential`）; 形式: `op://<vault名>/<アイテム名>/<フィールド名>` |
| 戻り値 | C.malloc で確保されたシークレット文字列のポインタ |
| エラー時 | クライアント初期化失敗またはシークレット解決失敗の場合、空文字列（`""`）を返す |

> **注意**: 使用後は必ず `FreeString()` で解放してください。戻り値が空文字列の場合はエラーが発生しています。

### `FreeString(s *C.char)`

`GetAPIKEY()` が返したC文字列のメモリを解放します。

## セキュリティについて

- シークレットはコードやファイルにハードコードせず、実行時に 1Password Vault から取得します。
- `myapp` はシークレットを標準出力に表示した直後に `memset()` でゼロクリアし、メモリダンプ等による漏洩リスクを低減しています。

## 依存ライブラリ

| ライブラリ | バージョン | 用途 |
|-----------|-----------|------|
| [onepassword-sdk-go](https://github.com/1password/onepassword-sdk-go) | v0.4.0 | 1Password 公式 Go SDK |
| [extism/go-sdk](https://github.com/extism/go-sdk) | v1.7.1 | SDK 内部の Wasm プラグイン実行 |
| [tetratelabs/wazero](https://github.com/tetratelabs/wazero) | v1.11.0 | Go 製 WebAssembly ランタイム |
