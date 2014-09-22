import urllib.request
import re
import math

picRoot = 'D:/pic/'

while True:
    url = 'http://www.qiushibaike.com/history'
    req = urllib.request.Request(url,headers={'User-Agent' : "Magic Browser"})
    response = urllib.request.urlopen(req)
    the_page = response.read().decode("UTF-8")
    pattern = re.compile(r'<span class="date">\n(.*?)\n</span>')
    date = pattern.findall(the_page)[0]
    print (date)
    pattern = re.compile(r'<div class="article block untagged mb15.*?<li class="share">',re.DOTALL)

    matchGroup = pattern.findall(the_page)

    for match in matchGroup:
        pattern = re.compile(r'<div class="article block untagged mb15" id=\'(.*?)\'>')
        id = pattern.findall(match)[0]
        pattern = re.compile(r'<div class="content" title="(.*?)">')
        time = pattern.findall(match)[0]
        pattern = re.compile(r'<div class="content" title=".*?">\n\n(.*?)\n\n</div>')
        content = pattern.findall(match)[0]
        pattern = re.compile(r'<div class="thumb">.*<img src="(.*?)" alt=',re.DOTALL)
        imgL = pattern.findall(match)
        if(imgL):
            img = imgL[0]
            path = picRoot + id + '.jpg'
            pic = urllib.request.urlretrieve(img,path)
        print (id + " " + time + "\n" + content + "\n")
        
    
