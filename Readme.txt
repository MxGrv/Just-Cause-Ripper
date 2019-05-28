Looks like:
1. tab-file "pc.tab" contains a list of records as if they were sequention in a single common archive:
General tab-file struct:
 Header:
  uint32 UNK {presumably, version}
  uint32 UNK {presumably, block size for arc-files, connected to this tab-file}
  uint32 UNK {presumably, amount of arc-files, connected to this tab-file}
 Records list: {one after another, no delimiters}
  Record:
   uint32 UNK {unknown, increasing; presumably, connected with mapping of archived files in the memory}
   uint32 UNK {presumably some sort of offset}
   uint32 size {archived file size}
2. arc-files "pc0.arc"..."pc4.arc" ...
// pc0.arc - distinguished, contains something else in the beginning of it

General sarc-record structure:
 Header:
  uint32 UNK {presumably, version}
  char[4] signature {fixed signature of "SARC"}
  uint32 UNK {probably, size of some information block for the file}
  uint32 UNK {probably, offset of the first file, relatively from the end of the sarc-record header}
   // Or offset of the first file beginning relatively from the end of sarc-record header, or offset, after (!) which data field begins; aligned by 16-bytes border
 List of file records: {one after another, no delimiters}
  File record:
   uint32 fileNameSize {file name size}
   char[fileNameSize] fileName {file name}
   uint32 offset {offset of the file beginning relatively from the sarc-record beginning}
   uint32 size {file size}
 // Space between SARC blocks are filled with 0x50 bytes

Sources:
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

Name, size and offset of resources in arc-files can be found in two files, connected with them:
First - .tab, with offsets and sizes
Second - .txt, with the list of all files' names in the .arc archive
But the list is some bigger, than the amount of resources in the arc-file; that's because duplicate files and files without extensions do not get saved to the arc-file
So first we need to remove these fake records
List contains links to the resources in arc-file
Resources are stored in arc-file and padded up to the block size, which is stated in the tab-file
*/

/*
Notes:
1. It is a lazy app: no optimization of any kind, Windows MSVC only, so it works rather slow. It might have some sort of defect (e.g., miss a real file per a SARC record... or not, IDK really) as well.
2. It works in a brutal manner: it is s**t-coded and it has awful perfomance, but it is light...
3. Nothing happens if there is more than one .tab-file found.
*/

/*
TODO (really):
- pc0.tab - seek for mutual link with .tab-file (seems to have exactly 7k files in operation)
- flag for text reports generation
*/
