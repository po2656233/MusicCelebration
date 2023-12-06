import requests
from bs4 import BeautifulSoup

def get_music_info(url):
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3'}
    response = requests.get(url, headers=headers)
    soup = BeautifulSoup(response.text, 'lxml')

    title_found = False
    for title in soup.find_all('title'):
        if title.is_title():
            music_title = title.string
            title_found = True
            break

    if not title_found:
        music_title = '未找到标题'

    music_info = {}
    try:
        music_artist = soup.find('h1', class_='music-artist').text
        music_lyric = soup.find('div', class_='music-lyric').text

        music_info = {'title': music_title, 'artist': music_artist, 'lyric': music_lyric}
    except:
        pass

    return music_info

# 其余代码不变

if __name__ == '__main__':
    music_url = 'https://example.com/music'  # 替换为你想抓取的音乐网页链接
    music_info = get_music_info(music_url)
    print(music_info)

    # 下载音乐
    music_title = music_info['title']
    music_artist = music_info['artist']
    music_filename = f'{music_title} - {music_artist}.mp3'
    download_music(music_url, music_filename)
    print(f'音乐 "{music_title} - {music_artist}" 已下载到 {music_filename}')
