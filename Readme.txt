По-видимому:
1. tab-файл "pc.tab" содержит список записей так, как если бы они шли последовательно в одном общем архиве:
Структура tab-файла общая:
 Заголовок:
  uint32 UNK {предположительно, версия}
  uint32 UNK {предположительно, размер блока для arc-файлов, связанных с этим tab-файлом}
  uint32 UNK {предположительно, количество arc-файлов, связанных с данным tab-файлов}
 Список записей: {одна за одной, без разделителей}
  Записи:
   uint32 UNK {неизвестно, но явно увеличивается, предположительно, связано с mapping-ом архивированных файлов в памяти}
   uint32 UNK {предположительно, некое смещение}
   uint32 size {размер файла-члена архива}
2. arc-файлы "pc0.arc"..."pc4.arc" ...
// pc0.arc - явно особенный

Структура sarc-записи:
 Заголовок:
  uint32 UNK {предположительно, версия}
  char[4] signature {фиксированная сигнатура "SARC"}
  uint32 UNK {неизвестно, но есть мнение, что размер некоего файлового информационного блока}
  uint32 UNK {неизвестно, но предположительно, смещение начала первого файла относительно конца заголовка sarc-записи}
   // Либо смещение начала первого файла относительно конца заголовка sarc-записи, либо смещение, после которого начинается область данных, выровенное по 16-байтной границе
 Список файловых записей: {одна за одной, без разделителей}
  Файловая запись:
   uint32 fileNameSize {размер имени файла}
   char[fileNameSize] fileName {имя файла}
   uint32 offset {смещение начала файла относительно начала sarc-записи}
   uint32 size {размер файла относительно начала sarc-записи}
 // Промежутки между блоками SARC заполняются байтами 0x50

Источники:
https://forum.xentax.com/viewtopic.php?t=2105
http://wiki.xentax.com/index.php/Just_Cause_SARC
http://wiki.xentax.com/index.php/Just_Cause_ARC
https://justcause.fandom.com/wiki/File_types_in_Avalanche_Engine_2.0

/*
Notes and Comments

The name, size and offsets of resources in ARC files can be found in two support files.
First, there's a *.tab file where you will find the offsets and sizes of the resources.
Second, there's a *.txt file that lists all the names of the files in the ARC archive.
However, the list is longer than there are resources in the ARC file. This is because duplicate files and files with no extension are not saved in the ARC file.
You should scan the list and remove those. If you do you should find that the list is chronologically accurate, from top to bottom, with regards to resources saved in the ARC file.
NOTE: Resources are stored in the ARC archive and padded up to pre-specified blocks. The size of these pre-specified blocks can be found in the TAB file (see specifications).

For example, if resource 'A' is 4000 in size, and files are padded in blocks of 2048 bytes, two blocks of 2048 would be reserved in the ARC file to store resource 'A':

2 * 2048 = 4096, 4096-4000= 96

Thus, after the last byte of the resource you will find 96 bytes used to fill the second block. The next resource would then start after those 96, i.e. at the third block.

Имя, размер и смещение ресурсов в arc-файлах может быть найдено в двух связанных с ними файлах
Первый - .tab, со смещениями и размерами ресурсов
Второй - .txt, со списком всех имен файлов в архиве arc
Но список несколько больше, чем количетсво ресурсов в arc-файле, потому что duplicate файлы и файлы без расширений не сохраняются в arc-файле
Поэтому нужно сначала удалить эти ложные записи
Список - хронологический, со ссылками на ресурсы, сохранные в arc-файле
Ресурсы хранятся в arc-файле и дополняются до размера блока, который также записан в tab-файле
*/

/*
Notes:
1. It is a lazy app: no optimization of any kind, Windows MSVC only, so it works rather slow. It might have some sort of defect (e.g., miss a real file per a SARC record... or not, IDK really) as well.
2. It works in a brutal manner: it is s**t-coded and it has awful perfomance, but it is light...
3. Nothing happens if there is more than one .tab-file found.
*/

/*
TODO (really):
- pc0.tab - глянуть сопоставления с .tab-файлом (в котором, видимо, оперируют всего 7 тысячами файлов (ровно))
- флаг генерации текстовых отчетов
*/
