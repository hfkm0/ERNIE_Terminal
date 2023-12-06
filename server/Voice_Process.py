# -*- coding: utf-8 -*-
import requests
import base64
import struct
import json
import time
import os

key = {}

# 原始数据
ORI_DATA_FILEPATH = 'audio.txt'
ORI_DATA_DONE_FLAG_FILEPATH = 'receive_done.txt'
# 原始数据转PCM文件
PCM_DATA_FILEPATH = 'pcm_data.pcm'
# Ernie对话结果文件
ERNIE_TXT_FILEPATH = 'ernie_result.txt'
# STOS生成的PCM文件
RESULT_DATA_FILEPATH = 'result_audio.pcm'
# 不使用STOS的Ernie对话结果文件
RESULT_TXT_FILEPATH = 'result_audio.txt'
# 处理完毕标志文件
DONE_TXT_FILEPATH = 'Voice_process_done.txt'
# 语音识别失败播报文件
VOICE_FAIL_TXT_FILEPATH = 'Voice_process_fail.pcm'

# 语音识别结果
SPR_Words = ""
# Ernie对话结果
ERNIE_Words = ""
# Ernie对话上下文
ernie_payload = json.dumps({
                    "messages": [{"role": "user", "content": "请使用纯中文且无特殊字符与我交流"},
                                {"role": "assistant", "content": "好的"}]
                })
# Ernie模型
ERNIE_Bot_4_0 = 'https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/completions_pro'
ERNIE_Bot_Turbo = 'https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/eb-instant'
ERNIE_Bot = 'https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/completions'

'''
    @brif    从指定json文件中获取key
    @param   FILEPATH    文件路径
    @return  key         key
'''
def Get_Key(FILEPATH):
    with open(FILEPATH, "r", encoding = 'utf-8') as f:
        key = json.load(f)
    return key

'''
    @brif    将上传的音频数据转换为PCM格式
    @param   FILEPATH    文件路径
    @return  none
    @note    PCM格式: 16位,小端,单声道,采样率16k
'''
def OriData_to_pcm(FILPATH = ORI_DATA_FILEPATH):
    #读取本地文件rec_0.txt
    with open(FILPATH, 'rb') as f:
        data = f.read()

    #16位合并
    data_int = [data[i+1] * 256 + data[i] - 32768 for i in range(0, len(data), 2)]

    # 将数据转换为二进制形式 大端为>h 小端为<h
    binary_data = b''.join([struct.pack('<h', data_) for data_ in data_int])

    # 将数据转换为二进制形式 大端为>h 小端为<h
    binary_data = b''.join([struct.pack('<h', data_) for data_ in data_int])

    # PCM文件头参数
    sample_rate = 16000 # 采样率
    bits_per_sample = 16 # 位深度
    channels = 1 # 通道数

    # 创建PCM文件头
    pcm_header = struct.pack('>ihhh', 0x46464952, sample_rate, channels, bits_per_sample)

    # 将数据和PCM文件头写入文件
    with open(PCM_DATA_FILEPATH, 'wb') as f:
        f.write(pcm_header)
        f.write(binary_data)

'''
    @brif    调用 百度短语音识别标准版API
    @param   key         key
    @param   dev_pid     语言模型
    @return  True False
    @note    识别结果保存在变量 SPR_Words 中
             dev_pid	语言	           模型	            是否有标点
             1537	    普通话(纯中文识别)	语音近场识别模型  有标点
             1737	    英语	           英语模型	        无标点
             1637	    粤语	           粤语模型	        有标点
             1837	    四川话	           四川话模型	    有标点
'''
def SPR_Req(key=key, dev_pid=1537):    
    global SPR_Words

    with open(PCM_DATA_FILEPATH, 'rb') as f:
        speech = f.read()
    
    FORMAT = 'pcm'
    RATE = '16000'
    CHANNEL = 1
    CUID = key['cuid']
    SPEECH = base64.b64encode(speech).decode('utf-8')

    url = 'https://vop.baidu.com/server_api'
    headers = {'Content-Type': 'application/json'}

    data = {
        'format': FORMAT,
        'rate': RATE,
        'channel': CHANNEL,
        'cuid': CUID,
        'len': len(speech),
        'speech': SPEECH,
        'token': key['voice_technology']['access_token'],
        'dev_pid':dev_pid
    }

    print('SPR正在识别...')
    response = requests.post(url, json=data, headers=headers).json()
    
    if 'result' in response:
        print('SPR识别结果:', response['result'][0])
        SPR_Words = response['result'][0]
        return True
    else:
        print('SPR识别失败')
        return False

'''
    @brif    调用 百度千帆大模型API
    @param   key         key
    @param   model       模型
    @return  True False
    @note    识别结果保存在变量 ERNIE_Words 中
             已开通的模型：
                ERNIE_Bot_Turbo     0.008元/千token
                ERNIE_Bot           0.012元/千token
                ERNIE_Bot_4_0       0.120元/千token
'''
def ERNIE_Req(key=key, model='ERNIE-Bot-turbo'):
    global ernie_payload
    global SPR_Words
    global ERNIE_Words
    
    if model == 'ERNIE_Bot_Turbo':
        url = ERNIE_Bot_Turbo
    elif model == 'ERNIE_Bot':
        url = ERNIE_Bot
    elif model == 'ERNIE_Bot_4_0':
        url = ERNIE_Bot_4_0
    else:
        print("ERNIE模型选择错误")
        return False

    error_times = 0

    url = url + "?access_token=" + key["large_scale_model"]["access_token"]
    headers = {'Content-Type': 'application/json'}
    
    data = json.loads(ernie_payload)
    ernie_payload_temp = json.dumps({
        "messages": data["messages"] + [
            {
                "role": "user",
                "content": SPR_Words
            }
        ]
    })

    while True:
        print("ERNIE等待回答...\n")
        response = requests.request("POST", url, headers=headers, data=ernie_payload_temp)

        if str(response.json().get("result")).find("None") != -1:
            print("ERNIE回答失败")
            print(response.json(), "\n")
            error_times += 1
            if error_times <= 2:
                continue
            return False
        else:
            data = json.loads(ernie_payload_temp)
            ernie_payload_temp = json.dumps({
                "messages": data["messages"] + [
                    {
                        "role": "assistant",
                        "content": response.json().get("result")
                    }
                ]
            })

            ernie_payload = ernie_payload_temp

            print("ERNIE回答如下:\n")
            print(response.json().get("result"), "\n")
            ERNIE_Words = response.json().get("result")

            with open(ERNIE_TXT_FILEPATH, "w") as f:
                f.write(ERNIE_Words)

            return True
    
'''
    @brif    调用 百度短文本在线合成标准版API
    @param   key        key
    @param   per        音色 基础语库 度小宇=1,度小美=0,度逍遥(基础)=3,度丫丫=4
    @return  True False
    @note    识别结果保存在 RESULT_DATA_FILEPATH 中
'''
def STOS_Req(key=key, per=4):
    global ERNIE_Words

    url = "https://tsn.baidu.com/text2audio"
    headers = {
            'Content-Type': 'application/x-www-form-urlencoded',
            'Accept': '*/*'
        }

    words = requests.utils.quote(ERNIE_Words)
    words = requests.utils.quote(words)

    # 合成的文本，使用UTF-8编码。不超过60个汉字或者字母数字。文本在百度服务器内转换为GBK后，长度必须小于120字节
    tex = str(words)
    # 客户端类型选择，web端填写固定值1
    ctp = str(1)
    # 固定值zh。语言选择,目前只有中英文混合模式，填写固定值zh
    lan = "zh"
    # 基础语库 度小宇=1，度小美=0，度逍遥（基础）=3，度丫丫=4
    per_in = str(per)
    # 语速，取值0-15，默认为5中语速
    spd = str(7)
    # 音调，取值0-15，默认为5中语调
    pit = str(5)
    # 音量，取值0-15，默认为5中音量（取值为0时为音量最小值，并非为无声）
    vol = str(5)
    # 3为mp3格式(默认)； 4为pcm-16k；5为pcm-8k；6为wav（内容同pcm-16k）
    # 注意aue=4或者6是语音识别要求的格式，但是音频内容不是语音识别要求的自然人发音，所以识别效果会受影响。
    aue = str(5)

    payload='tex='+tex+'&tok='+key['voice_technology']['access_token']+'&cuid='+key['cuid']+'&ctp='+ctp+\
            '&lan='+lan+'&spd='+spd+'&pit='+pit+'&vol='+vol+'&per='+per_in+'&aue='+aue

    print("STOS正在生成...")
    response = requests.request("POST", url, headers=headers, data=payload)

    if 'content' in response:
        print("STOS生成失败")
        return False
    else:
        print("STOS生成成功")    
        with open(RESULT_DATA_FILEPATH, "wb") as f:
            f.write(response.content)
        print("stos_result.pcm已生成")
        return True

'''
    @brief   从PCM文件中读取数据并转化为指定位宽数据列表返回
    @param   FILEPATH    文件路径
    @param   width       位宽
    @return  data
    @note    默认小端带签名
'''
def PCM_Read(FILEPATH , width):
    with open(FILEPATH, 'rb') as f:
        data_ori = f.read()

    if width == 16:
        data_hex = [data_ori[i+1] * 256  + data_ori[i]  for i in range(10, len(data_ori), 2)]
        data = [item - 32768 - 32768 for item in data_hex]

    for i in range(len(data)):
        if data[i] < -32768:
            data[i] = data[i] + 65536

    return data   

'''
    @brief   将PCM数据位数进行压缩
    @param   data_ori    数据列表
    @param   width_ori   原始位宽
    @param   width_new   目标位宽
    @param   mul         音频声音放大倍数 0-1为减小 1-无穷大 为放大
    @return  data
    @note    默认小端带签名
    @note    放大倍数过大或过小都会严重失真
'''
def PCM_Compress(data_ori, width_ori, width_new, mul=1):

    if width_ori == 16:
        if width_new == 12:
            data_new = [int((item / 16 - 2048 )* mul)  for item in data_ori]        

    return data_new

'''
    @brief   将PCM数据保存为TXT文件
    @param   FILEPATH
    @param   data       PCM数据,来自PCM_Read
    @param   width
    @return  none
'''
def PCM_to_TXTFile(FILEPATH, data, width):
    if width == 16:
        data_p = [data_ + 32768 for data_ in data]
        data_txt = b''.join([struct.pack('<H', data_) for data_ in data_p])
        with open(FILEPATH, 'wb') as f:
            f.write(data_txt)

'''
    @brif    删除生成的各种文件,复原现场
    @param   none
    @param   none
    @return  none
'''
def Files_Clear():
    if os.path.exists(ORI_DATA_FILEPATH + "_old") == True:
        os.rename(ORI_DATA_FILEPATH + "_old", ORI_DATA_FILEPATH)
    if os.path.exists(ERNIE_TXT_FILEPATH) == True:
        os.remove(ERNIE_TXT_FILEPATH)
    if os.path.exists(PCM_DATA_FILEPATH) == True:
        os.remove(PCM_DATA_FILEPATH)
    if os.path.exists(RESULT_DATA_FILEPATH) == True:
        os.remove(RESULT_DATA_FILEPATH)
    if os.path.exists(DONE_TXT_FILEPATH) == True:
        os.remove(DONE_TXT_FILEPATH)
    if os.path.exists(RESULT_TXT_FILEPATH) == True:
        os.remove(RESULT_TXT_FILEPATH)
    if os.path.exists(ORI_DATA_DONE_FLAG_FILEPATH) == True:
        os.remove(ORI_DATA_DONE_FLAG_FILEPATH)
        

def main_pcm():
    Files_Clear()
    
    key = Get_Key('./key.json')

    while True:
        print("等待语音数据...")
        while True:
            if os.path.exists(ORI_DATA_DONE_FLAG_FILEPATH) == True:
                break
            time.sleep(0.03)

        OriData_to_pcm()

        if SPR_Req(key) and ERNIE_Req(key) and STOS_Req(key):
            pass
        else:
            with open(VOICE_FAIL_TXT_FILEPATH, "rb") as f:
                voice_fail_data = f.read()
            with open(RESULT_TXT_FILEPATH, "wb") as f:
                f.write(voice_fail_data)
            continue

        with open(DONE_TXT_FILEPATH, "w") as f:
            f.write("done")

        os.rename(ORI_DATA_FILEPATH, ORI_DATA_FILEPATH + "_old")
        time.sleep(0.1)

def main_gbk():
    Files_Clear()
    
    key = Get_Key('./key.json')

    while True:
        print("等待语音数据...")
        while True:
            if os.path.exists(ORI_DATA_DONE_FLAG_FILEPATH) == True:
                break
            time.sleep(0.03)

        OriData_to_pcm()

        if SPR_Req(key) and ERNIE_Req(key, ERNIE_Bot_4_0):
            pass
        else:
            with open(ERNIE_TXT_FILEPATH, "w") as f:
                f.write("语音识别失败")

            with open(DONE_TXT_FILEPATH, "w") as f:
                f.write("done")

            os.rename(ORI_DATA_FILEPATH, ORI_DATA_FILEPATH + "_old")
            os.remove(ORI_DATA_DONE_FLAG_FILEPATH)
            continue

        with open(DONE_TXT_FILEPATH, "w") as f:
            f.write("done")

        os.rename(ORI_DATA_FILEPATH, ORI_DATA_FILEPATH + "_old")
        os.remove(ORI_DATA_DONE_FLAG_FILEPATH)
        time.sleep(0.1)

if __name__ == "__main__":
    # main_pcm()
    main_gbk()

