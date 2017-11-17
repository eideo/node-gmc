var qs = require('querystring');
var http = require('http');
var md5 = require('./md5');

/**
 * params:{
 *    trans_type：刷卡类型，长度2位
 *    pos_id：POS机号，长度8位，不足右补空格
 *    oper_id：POS员工号，长度8位，不足右补空格
 *    trans_id: 交易类型标志，长度2位
 *    amount: 金额，长度12位，没有小数点"."，精确到分，最后两位为小数位，不足左补0
 *    old_date：原交易日期，长度8位，格式yyyymmdd，退货时用，其他交易空格
 *    old_reference：原交易参考号，长度12位，退货时用，其他交易空格
 *    old_trace：原凭证号，长度6位，撤消时和重打印任意一笔时用（如重打印上一笔，则该值为000000），其他交易空格
 *    lrc：3位随机数字，交易验证数据，0~9的随机字符
 *    memo：备注，最大长度1500，非必填
 * }
 */
exports.paramsToRequest = function (params) {
    var request = '';
    request += padLeft(params.trans_type, 2, '0');
    request += padRight(params.pos_id, 8, ' ');
    request += padRight(params.oper_id, 8, ' ');
    request += padLeft(params.trans_id, 2, '0');
    request += padLeft((Number(params.amount) * 100) + '', 12, '0');
    request += padRight(params.old_date, 8, ' ');
    request += padRight(params.old_reference, 12);
    request += padLeft(params.old_trace, 6, '0');
    request += padLeft(params.lrc, 3, '0');
    if (params.memo) request += params.memo;
    console.log('{' + request + '}');
    return request;
}

/**
 * return params:{
 *     res_code：返回码，00表示成功，其它表示失败
 *     trans_type：刷卡类型
 *     trans_id: 交易类型标志
 *     bank_code：银行行号
 *     bank_name：银行名称
 *     card_no：卡号
 *     trace：凭证号
 *     amount：金额
 *     res_message：错误说明
 *     mer_id：商户号
 *     ter_id：终端号
 *     batch_no：批次号
 *     txn_date：交易日期
 *     txn_time：交易时间
 *     ref_data：交易参考号
 *     auth_code：授权号
 *     st_date：清算日期
 *     lrc：LRC校验
 *     memo：备注
 *     res_text：原始返回内容文本信息
 * }
 */
exports.responseToParams = function (response) {
    var params = {
        res_code: '', //返回码， 00 表示成功， 其它表示失败
        trans_type: '', //刷卡类型
        trans_id: '', //交易类型标志
        bank_code: '', //银行行号
        bank_name: '', //银行名称
        card_no: '', //卡号
        trace: '', //凭证号
        amount: '', //金额
        res_message: '', //错误说明
        mer_id: '', //商户号
        ter_id: '', //终端号
        batch_no: '', //批次号
        txn_date: '', //交易日期
        txn_time: '', //交易时间
        ref_data: '', //交易参考号
        auth_code: '', //授权号
        st_date: '', //清算日期
        lrc: '', //LRC校验
        memo: '', //备注
        res_text: '' //原始返回内容文本信息
    };

    if (response) {
        params.res_code = response.res_code.replace(/ /g, '');
        params.trans_type = response.trans_type.replace(/ /g, '');
        params.trans_id = response.trans_id.replace(/ /g, '');
        params.bank_code = response.bank_code.replace(/ /g, '');
        params.bank_name = response.bank_name.replace(/ /g, '');
        params.card_no = response.card_no.replace(/ /g, '');
        params.trace = response.trace.replace(/ /g, '');
        params.amount = response.amount.replace(/ /g, '');
        params.res_message = response.res_message.replace(/ /g, '');
        params.mer_id = response.mer_id.replace(/ /g, '');
        params.ter_id = response.ter_id.replace(/ /g, '');
        params.batch_no = response.batch_no.replace(/ /g, '');
        params.txn_date = response.txn_date.replace(/ /g, '');
        params.txn_time = response.txn_time.replace(/ /g, '');
        params.ref_data = response.ref_data.replace(/ /g, '');
        params.auth_code = response.auth_code.replace(/ /g, '');
        params.st_date = response.st_date.replace(/ /g, '');
        params.memo = response.memo.replace(/ /g, '');
        params.lrc = response.lrc.replace(/ /g, '');
        params.res_text = response.res_text;
    }
    return params;
}

function padLeft(string, length, chars) {
    length = length || 0;
    let _chars = chars || ' ';

    if (string && length > 0 && string.length < length) {
        for (var i = 1; i < length; i++) {
            chars += _chars;
        }
        return chars.substring(0, chars.length - string.length) + string;
    } else {
        return string;
    }
}

function padRight(string, length, chars) {
    length = length || 0;
    let _chars = chars || '';

    if (string && length > 0 && string.length < length) {
        for (var i = 1; i < length; i++) {
            chars += _chars;
        }
        return string + chars.substring(0, chars.length - string.length);
    } else {
        return string;
    }
}

/**
 * 把对象所有元素，按照“参数=参数值”的模式用“&”字符拼接成字符串
 * @param para 需要拼接的对象
 * return 拼接完成以后的字符串
 */
exports.createLinkstring = function (para) {
    //return qs.stringify(para);
    var ls = '';
    for (var k in para) {
        ls = ls + k + '=' + para[k] + '&';
    }
    ls = ls.substring(0, ls.length - 1);
    return ls;
}

/**
 * 把对象所有元素，按照“参数=参数值”的模式用“&”字符拼接成字符串，并对字符串做urlencode编码
 * @param para 需要拼接的对象
 * return 拼接完成以后的字符串
 */
exports.createLinkstringUrlencode = function (para) {
    return qs.stringify(para);
}

/**
 * 除去对象中的空值和签名参数
 * @param para 签名参对象
 * return 去掉空值与签名参数后的新签名参对象
 */
exports.paraFilter = function (para) {
    var para_filter = new Object();
    for (var key in para) {
        if (key == 'sign' || key == 'sign_type' || para[key] == '') {
            continue;
        } else {
            para_filter[key] = para[key];
        }
    }

    return para_filter;
}

/**
 * 对对象排序
 * @param para 排序前的对象
 * return 排序后的对象
 */
exports.argSort = function (para) {
    var result = new Object();
    var keys = Object.keys(para).sort();
    for (var i = 0; i < keys.length; i++) {
        var k = keys[i];
        result[k] = para[k];
    }
    return result;
}

exports.buildRequestPara = function (para_temp, key) {
    //除去待签名参数数组中的空值和签名参数
    var para_filter = this.paraFilter(para_temp);

    //对待签名参数数组排序
    var para_sort = this.argSort(para_filter);

    //签名结果与签名方式加入请求提交参数组中
    para_sort['sign'] = md5.md5Sign(this.createLinkstring(para_sort), key);

    return para_sort;
}


/**
 * 远程获取数据，POST模式
 * @param url 指定URL完整路径地址
 * @param para 请求的数据
 * @param input_charset 编码格式。默认值：空值
 * return 远程输出的数据
 */
exports.getHttpResponsePOST = function (config, para, callback) {
    var parsed_url = require('url').parse(config.url);
    var para_str = JSON.stringify(para);

    var options = {
        host: parsed_url.hostname,
        port: parsed_url.port,
        path: parsed_url.path,
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-from-urlencode;charset=UTF-8',
            'Content-Length': para_str.length
        }
    };

    var req = http.request(options, function (res) {
        var responseText = '';
        res.on('data', function (chunk) {
            responseText += chunk;
        });
        res.on('end', function () {
            callback && callback(responseText);
        });
    });

    req.write(para_str);
    req.end();
}