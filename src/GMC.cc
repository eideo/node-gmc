#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <uv.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <windows.h>

using namespace v8;
using namespace std;

struct DelayBaton
{
    // required
    uv_work_t request;                    // libuv
    uv_async_t async_request;             // libuv
    Persistent<Function> cbDataCallback;  // javascript data callback
    Persistent<Function> cbErrorCallback; // javascript error callback

    char *status;

    char requestData[2048];
    char responseData[2048];

    // char *res_code = new char[2];     //返回码， 00 表示成功， 其它表示失败
    // char *trans_type = new char[2];   //刷卡类型
    // char *trans_id = new char[2];     //交易类型标志
    // char *bank_code = new char[4];    //银行行号
    // char *bank_name = new char[8];    //银行名称
    // char *card_no = new char[20];     //卡号
    // char *trace = new char[6];        //凭证号
    // char *amount = new char[12];      //金额
    // char *res_message = new char[40]; //错误说明
    // char *mer_id = new char[15];      //商户号
    // char *ter_id = new char[8];       //终端号
    // char *batch_no = new char[6];     //批次号
    // char *txn_date = new char[4];     //交易日期
    // char *txn_time = new char[6];     //交易时间
    // char *ref_data = new char[12];    //交易参考号
    // char *auth_code = new char[6];    //授权号
    // char *st_date = new char[4];      //清算日期
    // char *lrc = new char[3];          //LRC校验
    // char *memo = new char[1500];      //备注
    // char *res_text = new char[2000];  //原始返回内容文本信息
};

DelayBaton *baton;

static std::string GBKToUTF8(const std::string &strGBK)
{
    static std::string strOutUTF8 = "";
    WCHAR *str1;
    int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
    str1 = new WCHAR[n];
    MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
    n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
    char *str2 = new char[n];
    WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
    strOutUTF8 = str2;
    delete[] str1;
    str1 = NULL;
    delete[] str2;
    str2 = NULL;
    return strOutUTF8;
}

static char *substring(char *ch, int pos, int length)
{
    char *pch = ch;
    //定义一个字符指针，指向传递进来的ch地址。
    char *subch = (char *)calloc(sizeof(char), length + 1);
    //通过calloc来分配一个length长度的字符数组，返回的是字符指针。
    int i;
    //只有在C99下for循环中才可以声明变量，这里写在外面，提高兼容性。
    pch = pch + pos;
    //是pch指针指向pos位置。
    for (i = 0; i < length; i++)
    {
        subch[i] = *(pch++);
        //循环遍历赋值数组。
    }
    subch[length] = '\0'; //加上字符串结束符。
    return subch;         //返回分配的字符数组地址。
}

// libuv异步线程函数
void Delay(uv_work_t *req)
{
    DelayBaton *baton = static_cast<DelayBaton *>(req->data);

    HINSTANCE hDllInst = LoadLibrary("c:\\gmc\\posinf.dll");
    if (hDllInst)
    {
        int result;

        typedef int(WINAPI * lpAddFun)(char *, char *);
        lpAddFun bankall = NULL;
        bankall = (lpAddFun)GetProcAddress(hDllInst, "bankall");
        if (bankall)
        {
            result = bankall(baton->requestData, baton->responseData);

            if (result == 0)
            {
                baton->status = "00";
            }
            else
            {
                baton->status = "501";
            }
        }
        else
        {
            baton->status = "500";
        }
    }
    else
    {
        baton->status = "404";
    }
}

// libuv异步事件循环完成回调函数
void DelayAsyncAfter(uv_work_t *req, int status)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    // 获取引用上下文
    DelayBaton *baton = static_cast<DelayBaton *>(req->data);

    if (baton->status == "00")
    {
        Local<Object> outInfo = Object::New(isolate);
        outInfo->Set(String::NewFromUtf8(isolate, "res_code"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 0, 2)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "trans_type"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 2, 2)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "trans_id"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 4, 2)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "bank_code"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 6, 4)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "bank_name"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 10, 8)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "card_no"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 18, 20)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "trace"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 38, 6)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "amount"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 44, 12)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "res_message"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 56, 40)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "mer_id"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 96, 15)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "ter_id"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 111, 8)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "batch_no"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 119, 6)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "txn_date"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 125, 4)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "txn_time"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 129, 6)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "ref_data"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 135, 12)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "auth_code"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 147, 6)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "st_date"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 153, 4)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "lrc"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 1657, 3)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "memo"), String::NewFromUtf8(isolate, GBKToUTF8(substring(baton->responseData, 157, 1500)).c_str()));
        outInfo->Set(String::NewFromUtf8(isolate, "res_text"), String::NewFromUtf8(isolate, GBKToUTF8(baton->responseData).c_str()));

        // 获取回调函数
        Local<Function> cb = Local<Function>::New(isolate, baton->cbDataCallback);
        const unsigned argc = 1;
        Local<Value> argv[argc] = {outInfo};
        cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
    else
    {
        // 获取回调函数
        Local<Function> cb = Local<Function>::New(isolate, baton->cbErrorCallback);
        const unsigned argc = 1;
        Local<Value> argv[argc] = {String::NewFromUtf8(isolate, baton->status)};
        cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
}

void BankAll(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Object> bufferObj = args[0]->ToObject();
    char *bufferData = node::Buffer::Data(bufferObj);
    size_t bufferLength = node::Buffer::Length(bufferObj);

    baton = new DelayBaton;
    memset(baton->requestData, 0, sizeof(baton->requestData));
    memset(baton->responseData, 0, sizeof(baton->responseData));
    memcpy(baton->requestData, bufferData, bufferLength);
    baton->cbDataCallback.Reset(isolate, Persistent<Function>::Persistent(isolate, Local<Function>::Cast(args[1])));
    baton->cbErrorCallback.Reset(isolate, Persistent<Function>::Persistent(isolate, Local<Function>::Cast(args[2])));
    baton->request.data = baton;

    uv_queue_work(uv_default_loop(), &baton->request, Delay, DelayAsyncAfter);

    args.GetReturnValue().Set(Number::New(isolate, 1));
}

void Init(Handle<Object> exports)
{
    NODE_SET_METHOD(exports, "bankall", BankAll);
}

NODE_MODULE(addon, Init)