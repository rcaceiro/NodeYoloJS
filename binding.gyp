{
 "variables":{
  "with_opencv%":"<!(node ./util/has_lib.js opencv)",
  "with_cuda%":"<!(node ./util/has_lib.js cuda)"
 },
 "targets":[
  {
   "target_name":"nodeyolojs",
   "sources":[
    "src/module.cpp"
   ],
   "libraries":[
    "<(module_root_dir)/yolo/libyolo.a"
   ],
   "defines":[
    "NAPI_DISABLE_CPP_EXCEPTIONS"
   ],
   "include_dirs": [
       "<(module_root_dir)/yolo/src",
       "<(module_root_dir)/darknet/src",
       "<(module_root_dir)/darknet/include"
    ],
   "cflags":[
    "-Wall",
    "-Wfatal-errors",
    "-fPIC"
   ],
   "conditions":[
    [
	'with_opencv=="true"',
	{
	 "defines":[
	  "OPENCV"
	 ],
	 "libraries":[
	  "-lopencv_core",
	  "-lopencv_highgui"
	 ]
	}
    ],
    [
	'with_cuda=="true"',
	{
	 "defines":[
	  "GPU"
	 ],
	 "libraries":[
	  "-L/usr/local/cuda/lib64",
	  "-lcuda",
	  "-lcudart",
	  "-lcublas",
	  "-lcurand",
	  "-lcudnn"
	 ],
	 "include_dirs":[
	  "/usr/local/cuda/include"
	 ]
	}
    ]
   ]
  }
 ]
}