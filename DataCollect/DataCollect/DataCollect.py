import urllib
import urllib2
import re
import math
import MySQLdb
import string
import sys
#coding=utf-8

reload(sys) 
sys.setdefaultencoding('utf-8')
picRoot = 'D:/pic/'
conn= MySQLdb.connect(host='127.0.0.1',user='root',passwd='345920',db='laughterDB',port=3306)
conn.set_character_set('utf8')
cursor = conn.cursor()
print '�������ݿ�ɹ���'
ii = 0
j=0
myfile = open('D:testit.txt', 'a')
while True:
    url = 'http://www.qiushibaike.com/history'
    req = urllib2.Request(url,headers={'User-Agent' : "Magic Browser"})
    response = urllib2.urlopen(req)
    the_page = response.read().decode("UTF-8")

    pattern = re.compile(r'<span class="date">\n(.*?)\n</span>')
    date = pattern.findall(the_page)[0]
    pattern = re.compile(r'\d*')
    dateList = pattern.split(date)
    date = date.replace(dateList[1],"-")
    date = date.replace(dateList[2],"-")
    date = date.replace(dateList[3],"")
    count = cursor.execute('select * from datetotal where date_qiushi = \''+date+'\'' )
    if(count != 0):
        continue
    sql = 'insert into datetotal(date_qiushi) values(\''+date+'\')'
    cursor.execute(sql)
    conn.commit()
    pageNum = 1
    pattern = re.compile(r'.*<a href="/history/page/(.*?)\?s=',re.DOTALL)
    pages = pattern.findall(the_page)
    weiba = ''
    page = 0
    if(pages):
        page = string.atoi(pages[0])
        page = page/2
        pattern = re.compile(r'<a href="/history/page/\d*(.*?)">\d*</a>')
        weiba = pattern.findall(the_page)[0]
    while True:

        pattern = re.compile(r'<div class="article block untagged mb15.*?<li class="share">',re.DOTALL)

        matchGroup = pattern.findall(the_page)

        for match in matchGroup:
            pattern = re.compile(r'<div class="article block untagged mb15" id=\'(.*?)\'>')
            id = pattern.findall(match)[0]
            pattern = re.compile(r'<div class="content" title="(.*?)">')
            time = pattern.findall(match)[0]
            pattern = re.compile(r'<div class="content" title=".*?">\n\n(.*?)\n\n</div>')
            content = pattern.findall(match)[0]
            content = content.replace('\'','\"')
            pattern = re.compile(r'<div class="thumb">.*<img src="(.*?)" alt=',re.DOTALL)
            imgL = pattern.findall(match)
            path = ''
            if(imgL):
                img = imgL[0]
                path = picRoot + id + '.jpg'
                pic = urllib.urlretrieve(img,path)
            pageStr = '%d'%pageNum
            sql = 'insert into qiushi(id_qiushi,time_qiushi,content,picPath,page) values(\''+id+'\',\''+time+'\',\''+content+'\',\''+path+'\',\''+pageStr+'\')'
            try:
                cursor.execute(sql)
                if ii == 10:
                    j = j+10
                    conn.commit()
                    str = '���ύ����:%d'%j
                    print str
                    ii=0
                ii = ii+1
            except :
                myfile.write(sql+'\n')
                myfile.close()
                myfile = open('D:testit.txt', 'a')
        pageNum = pageNum + 1
        if(pageNum > page):
            break
        url = 'http://www.qiushibaike.com/history/page/%d'%pageNum + weiba
        req = urllib2.Request(url,headers={'User-Agent' : "Magic Browser"})
        response = urllib2.urlopen(req)
        the_page = response.read().decode("UTF-8")

#import MySQLdb
 
#conn= MySQLdb.connect(host='127.0.0.1',user='root',passwd='345920',db='test',port=3306)
#cur=conn.cursor()
#cur.execute('select * from tt')
#for id in cur.fetchall():
#    print id
#cur.close()
#conn.close()

