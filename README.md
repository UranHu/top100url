# top100url
## Phase 1
Divide large input file into small pieces.  
Those splited files don't have to be smaller than FLAGS_block_size(defualt value 256MB).  
Just make sure the size of distinc urls in on file is no larger then 1G - FLAGS_block_size.  
Under such case, the memory usage is still safe(Actually not, cause std::shard_ptr and integers used to record every url emerged times also need spcae, but I ingore them here).  

If it doesn't, which mean at least 3M urls(768Mb and each url is no larger than 256B) are in the same sharding and there are distinct with each other.  
For such extreme strange input file, EMMM.  

Phase 1 reads single block from input file one by one.  
Use several worker threads(turl_split) to concurrently split these block.  
But the overhead of synchronization is large.  

## Phase 2
Read small files to pick top_k most often emerged urls in these files.  
Since Phase 1 re-arrange urls by their hash values, each distinc url only stays in one splited file.  
The reading and counting of Phase 2 is very simillar to Phase 1, but with heavier sydnchronization.  

For each file,   
reader reads one block -> several counter count -> reader continues reading -> ... -> counter count the final block -> counters sorts urls -> map statics top_k urls for now  
those procedures should be strictly sequential, which make me use a lot of atomic variables.  

So I have some doubts about is multi-thread in this scenario worthy.  

## Configuration
Flags_worker_num'd better equal with the number of cpu cores.  

## Test
turl_reader, turl_counter, turl_split and turl_map are rigorously designed for Phase 1 and 2. It's hard for me to desing a unit test for each of them.  
I have use simple input file to test them as a whole item.  

## Input fotmat
Input file shold be strictly formed as one line for one url.  
Command line arguments are parsed by gflags frame.  

## Output
If use turl_top.sh, there will be a log file stores log and ans.  
If execute the execitable file, ans and log are outputed to stdin and stderr respectively. And user must assure there is a temp dir undre current dir.  