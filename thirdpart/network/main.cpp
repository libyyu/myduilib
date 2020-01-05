// #include <iostream>
// #include <map>
// #include "./src/base/CMemPool.h"
// 
// 
// void main()
// {
// 	typedef std::map<int,void*>   mapt;
// 	mapt _map;
// 	Xiami::CMemPool<16,16*64*2>   _pool;
// 	for (int k =0;k<3;++k)
// 	{
// 		for (size_t i = 1;i <= 16*64;++i)
// 		{
// 			void* p = _pool.Alloc(i);
// 			auto it = _map.find(i);
// 			if(it != _map.end())
// 			{
// 				assert(p == it->second);
// 			}
// 			else
// 			{
// 				_map.insert(std::make_pair(i,p));
// 			}
// 
// 			_pool.Free(p);
// 		}
// 	}
// 
// 	system("pause");
// }
