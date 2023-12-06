import requests
import json

API_KEY = ""
SECRET_KEY = ""

'''
    @brif    更新指定技术类型key
    @param   key         key
    @param   tech        技术类型   eg: "voice_technology" "large_scale_model"
    @param   API_KEY     API_KEY
    @param   SECRET_KEY  SECRET_KEY
    @return  none
'''
def Update_Tech_Key(key, tech, API_KEY, SECRET_KEY):
    url = "https://aip.baidubce.com/oauth/2.0/token?client_id=" + key[tech]["API_KEY"] + "&client_secret=" + key[tech]["SECRET_KEY"] + "&grant_type=client_credentials"
    
    payload = json.dumps("")
    headers = {
        'Content-Type': 'application/json',
        'Accept': 'application/json'
    }
    
    response = requests.request("POST", url, headers=headers, data=payload)
    response = response.json()
    key[tech]["refresh_token"] = response["refresh_token"]
    key[tech]["expires_in"] = response["expires_in"]
    key[tech]["session_key"] = response["session_key"]
    key[tech]["access_token"] = response["access_token"]
    key[tech]["scope"] = response["scope"]
    key[tech]["session_secret"] = response["session_secret"]


def main():
    # 读取key.json
    with open("key.json", "r", encoding = 'utf-8') as f:
        key = json.load(f)
    
    Update_Tech_Key(key, "voice_technology", API_KEY, SECRET_KEY)
    Update_Tech_Key(key, "large_scale_model", API_KEY, SECRET_KEY)

    with open("key.json", "w", encoding = 'utf-8') as f:
        json.dump(key, f, ensure_ascii=False, indent=4)
    

if __name__ == '__main__':
    main()
