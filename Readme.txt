��-��������:
1. tab-���� "pc.tab" �������� ������ ������� ���, ��� ���� �� ��� ��� ��������������� � ����� ����� ������:
��������� tab-����� �����:
 ���������:
  uint32 UNK {����������������, ������}
  uint32 UNK {����������������, ������ ����� ��� arc-������, ��������� � ���� tab-������}
  uint32 UNK {����������������, ���������� arc-������, ��������� � ������ tab-������}
 ������ �������: {���� �� �����, ��� ������������}
  ������:
   uint32 UNK {����������, �� ���� �������������, ����������������, ������� � mapping-�� �������������� ������ � ������}
   uint32 UNK {����������������, ����� ��������}
   uint32 size {������ �����-����� ������}
2. arc-����� "pc0.arc"..."pc4.arc" ...
// pc0.arc - ���� ���������

��������� sarc-������:
 ���������:
  uint32 UNK {����������������, ������}
  char[4] signature {������������� ��������� "SARC"}
  uint32 UNK {����������, �� ���� ������, ��� ������ ������� ��������� ��������������� �����}
  uint32 UNK {����������, �� ����������������, �������� ������ ������� ����� ������������ ����� ��������� sarc-������}
   // ���� �������� ������ ������� ����� ������������ ����� ��������� sarc-������, ���� ��������, ����� �������� ���������� ������� ������, ���������� �� 16-������� �������
 ������ �������� �������: {���� �� �����, ��� ������������}
  �������� ������:
   uint32 fileNameSize {������ ����� �����}
   char[fileNameSize] fileName {��� �����}
   uint32 offset {�������� ������ ����� ������������ ������ sarc-������}
   uint32 size {������ ����� ������������ ������ sarc-������}
 // ���������� ����� ������� SARC ����������� ������� 0x50

���������:
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

���, ������ � �������� �������� � arc-������ ����� ���� ������� � ���� ��������� � ���� ������
������ - .tab, �� ���������� � ��������� ��������
������ - .txt, �� ������� ���� ���� ������ � ������ arc
�� ������ ��������� ������, ��� ���������� �������� � arc-�����, ������ ��� duplicate ����� � ����� ��� ���������� �� ����������� � arc-�����
������� ����� ������� ������� ��� ������ ������
������ - ���������������, �� �������� �� �������, ��������� � arc-�����
������� �������� � arc-����� � ����������� �� ������� �����, ������� ����� ������� � tab-�����
*/

/*
Notes:
1. It is a lazy app: no optimization of any kind, Windows MSVC only, so it works rather slow. It might have some sort of defect (e.g., miss a real file per a SARC record... or not, IDK really) as well.
2. It works in a brutal manner: it is s**t-coded and it has awful perfomance, but it is light...
3. Nothing happens if there is more than one .tab-file found.
*/

/*
TODO (really):
- pc0.tab - ������� ������������� � .tab-������ (� �������, ������, ��������� ����� 7 �������� ������ (�����))
- ���� ��������� ��������� �������
*/
