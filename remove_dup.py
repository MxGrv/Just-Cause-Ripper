#! python3

# Crawl over the current directory and remove all identical files

import os
import sys
import hashlib

target = os.getcwd() + '\\result'
dup_suf = '_MY_DUP'

for folderName, subfolders, fileNames in os.walk(target):
  for fileName in fileNames:

    filePath = os.path.join(folderName, fileName)

    hash = hashlib.md5()
    with open(filePath, "rb") as f:
      for chunk in iter(lambda: f.read(4096), b""):
        hash.update(chunk)

    fileBase, fileExt = os.path.splitext(fileName)

    digest = hash.digest()

    for folderNameToo, subfoldersToo, fileNamesToo in os.walk(target):
      for fileNameToo in fileNamesToo:

        fileBaseToo, fileExtToo = os.path.splitext(fileNameToo)

        isOk = (fileName == fileNameToo)
        # Watch "_MY_DUP" endings (!)
        if (fileExt == fileExtToo + dup_suf) or (fileExtToo == fileExt + dup_suf):
          isOk = True

        if folderName != folderNameToo and fileName == fileNameToo:

          filePathToo = os.path.join(folderNameToo, fileNameToo)

          hashToo = hashlib.md5()
          with open(os.path.join(folderNameToo, fileNameToo), "rb") as ft:
            for chunkToo in iter(lambda: ft.read(4096), b""):
              hashToo.update(chunkToo)

          digestToo = hashToo.digest()

          if digest == digestToo:

            print('Removing: ' + filePathToo + ' as ' + filePath)

            os.remove(filePathToo)
