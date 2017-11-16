##通联支付MIS系统Node.js Addon模块
# 1、bankall,银联统一支付接口函数
    参数{
        trans_type：刷卡类型，长度2位（00：银行卡，16：POS通）
        pos_id：POS机号，长度8位，不足右补空格
        oper_id：POS员工号，长度8位，不足右补空格
        trans_id: 交易类型标志，长度2位（00：消费，01：撤销，02：退货，03：查余额，04：重打印，05：签到，06：结算，07：重打结算单）
        amount: 金额，长度12位，没有小数点"."，精确到分，最后两位为小数位，不足左补0
        old_date：原交易日期，长度8位，格式yyyymmdd，退货时用，其他交易空格
        old_reference：原交易参考号，长度12位，退货时用，其他交易空格
        old_trace：原凭证号，长度6位，撤消时和重打印任意一笔时用（如重打印上一笔，则该值为000000），其他交易空格
        lrc：3位随机数字，交易验证数据，0~9的随机字符
        memo：备注，最大长度1500，非必填
    }
# 2、qrcode,扫码支付接口
   C扫B参数
   {
		"gProduct": "1",
		"gPlatform": "1",
		"gEnv": "1",
		"gVersion": "1.0.0",
		"actCode": "3911",
		"reqTime": "2017-11-15 15:40:22",
		"mcCode": "225",
		"tranTime": "2017-11-15 15:40:22",
		"billNo": "3064201710112909507031419656",
		"totalAmount": "1",
		"msgId": "703140955123456",
		"regID": "RegID",
		"memberId": "VipNo",
		"counterNo": "CounterNo",
		"notifyUrl": "http://222.240.182.134:8899/repeatplat/repeat/testPayCallBack",
		"returnUrl": "https://www.baidu.com/s?a=1",
		"mid": "123456789900080",
		"tid": "12345080"
	}
  订单查询参数
  {
    "gProduct": "1",
    "gPlatform": "1",
    "gEnv": "1",
    "gVersion": "1.0.0",
	"actCode": "3913",
	"reqTime": "2017-11-15 15:40:22",
	"mcCode": "225",
	"tranTime": "2017-11-15 15:40:22",
	"billNo": "3064201710112909507031419656",
	"mid": "123456789900080",
	"tid": "12345080"
}
