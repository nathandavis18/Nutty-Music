import sys
sys.path.append("../Python Libs")
from ytmusicapi import YTMusic

yt = YTMusic()

searchResults = yt.search(sys.argv[1], filter='songs')
writeToFile = ""
for i in range(5):
    song = searchResults[i]['title']
    artist = searchResults[i]['artists'][0]['name']
    url = searchResults[i]['videoId']

    writeToFile += song + ' - ' + artist + ' BREAKPOINT ' + url + '\n'

file = open("searchResults.txt", 'w', encoding='utf-8')
file.write(writeToFile)
file.close()