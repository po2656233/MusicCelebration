# This Python file uses the following encoding: utf-8

import os
from pathlib import Path
import sys
import requests


from bs4 import BeautifulSoup
from selenium import webdriver
from PySide2.QtWidgets import QApplication, QWidget, QPushButton, QTextEdit
from PyQt5.QtCore import QThread
from PySide2.QtCore import QFile
from PySide2.QtUiTools import QUiLoader
from PySide2.QtCore import QTimer
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service

import asyncio

headers = {
    "User-Agent":
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36 Edg/119.0.0.0"
    }

params = {
        'authority': 'www.kugou.com',
        'method': 'GET',
        'path': '/mixsong/1hiccibc.html',
        'scheme': 'https'
    }

class Widget(QWidget):
    def __init__(self):
        super(Widget, self).__init__()
        self.MyTimer = QTimer()
        self.index = 0
        self.songs = ''
        self.load_ui()
        self.crawler = self.findChild(QPushButton, "pushButton")
        self.closeWg = self.findChild(QPushButton, "pushButton_2")
        self.textEdit = self.findChild(QTextEdit, "textEdit")
#        self.crawler.clicked.connect(self.get_info)
        self.crawler.clicked.connect(self.startTest)
        self.closeWg.clicked.connect(self.close)

    def get_info(self):
        self.index += 1
        host = "https://www.kugou.com/yy/rank/home/"
        url = host+"{}-8888.html".format(self.index)
        web_data = requests.get(url, headers=headers)
        soup = BeautifulSoup(web_data.text, 'lxml')
        ranks = soup.select('span.pc_temp_num')
        titles = soup.select('div.pc_temp_songlist > ul > li > a')
        times = soup.select('span.pc_temp_tips_r > span')
        for rank, title, time in zip(ranks, titles, times):
            data = {
                "rank": rank.get_text().strip(),
                "singer": title.get_text().replace("\n", "").replace("\t", "").split('-')[1],
                "song": title.get_text().replace("\n", "").replace("\t", "").split('-')[0],
                "time": time.get_text().strip(),
                "url": title.get('href')
            }
            print("tile", title.get('href'))
            strData = str(data)
            self.textEdit.append(strData)
            # 获取歌曲
            songs_url = title.get('href')
            if songs_url != '':
                self.songs += songs_url + ','

        if self.index == 23:
            print("开启线程，获取歌曲地址")
            async_thread = AsyncCustomThread(self)
            async_thread.set_url(self.songs)
            #        async_thread.finished.connect(async_thread.deleteLater)
            async_thread.finished.connect(self.async_task_finished)
            #        async_thread.quit()
            async_thread.wait()
            async_thread.start()
            async_thread.exec()
            self.MyTimer.stop()

    def startTest(self):
        self.MyTimer.start(100)
        self.MyTimer.timeout.connect(self.get_info)

    def load_ui(self):
        loader = QUiLoader()
        path = os.fspath(Path(__file__).resolve().parent / "form.ui")
        ui_file = QFile(path)
        ui_file.open(QFile.ReadOnly)
        loader.load(ui_file, self)
        ui_file.close()

    def start_async_task(self, url):
        print("正在加载", url)

    def async_task_finished(self):
        print("异步任务已完成")


class AsyncCustomThread(QThread):
    def __init__(self, parent=None):
        super(QThread, self).__init__()
        # 不开网页搜索
        chrome_options = Options()
        # 设置chrome浏览器无界面模式
        chrome_options.add_argument('--headless')
        s = Service(executable_path=r"C:\\Program Files\\Google\Chrome\\Application\\chromedriver.exe")
        self.browser = webdriver.Chrome(service=s, options=chrome_options)

    def __del__(self):
        print('Stop thread task running.')
        self.browser.close()
        self.terminate()

    def set_url(self, url):
        self.url = url

    def run(self):
        asyncio.run(self.get_music())
#        asyncio.run(self.async_task())
#    async def async_task(self):
#        await asyncio.sleep(2)  # 模拟一个耗时的异步任务
#        print("异步任务完成")

    async def get_music(self):
        songslist = self.url.split(',')
        try:
            for songurl in songslist:
                self.browser.get(songurl)
                await asyncio.sleep(0.6)
                audio = self.browser.find_element(By.CLASS_NAME, "music")
                print("结果: ", audio.get_attribute('src'))
                file_path = 'music_list.txt'
                with open(file_path, mode='a', encoding='utf-8') as file_obj:
                    file_obj.write(audio.get_attribute('src')+'\n')
        finally:
            return


if __name__ == "__main__":
    app = QApplication([])
    widget = Widget()
    widget.show()
    sys.exit(app.exec_())

