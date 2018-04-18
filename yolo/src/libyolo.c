#include "libyolo.h"

void print_yolo_detections(FILE **fps, char *id, int total, int classes, int w, int h, detection *dets)
{
 int i, j;
 for(i=0; i<total; ++i)
 {
  float xmin=dets[i].bbox.x-dets[i].bbox.w/2.;
  float xmax=dets[i].bbox.x+dets[i].bbox.w/2.;
  float ymin=dets[i].bbox.y-dets[i].bbox.h/2.;
  float ymax=dets[i].bbox.y+dets[i].bbox.h/2.;

  if(xmin<0)
  {
   xmin=0;
  }
  if(ymin<0)
  {
   ymin=0;
  }
  if(xmax>w)
  {
   xmax=w;
  }
  if(ymax>h)
  {
   ymax=h;
  }

  for(j=0; j<classes; ++j)
  {
   if(dets[i].prob[j])
   {
    fprintf(fps[j], "%s %f %f %f %f %f\n", id, dets[i].prob[j], xmin, ymin, xmax, ymax);
   }
  }
 }
}

void yolo_cleanup(yolo_object *yolo)
{
 free_network(yolo->net);
 free(yolo->names);
 free(yolo);
 yolo_object **ptr_yolo=&yolo;
 (*ptr_yolo)=NULL;
}

yolo_object *yolo_init(char *workingDir, char *datacfg, char *cfgfile, char *weightfile)
{
 clock_t time=clock();

 yolo_object *yolo=(yolo_object *)malloc(sizeof(yolo_object));
 if(!yolo)
 {
  return NULL;
 }
 memset(yolo, 0, sizeof(yolo_object));

 char cur_dir[1024];
 getcwd(cur_dir, sizeof(cur_dir));
 if(chdir(workingDir) == -1)
 {
  fprintf(stderr, "%s\n", strerror(errno));
  return NULL;
 }

 yolo->net=load_network(cfgfile, weightfile, 0);

 list *options=read_data_cfg(datacfg);
 char *name_list=option_find_str(options, "names", "data/names.list");
 yolo->names=get_labels(name_list);

 set_batch_network(yolo->net, 1);
 srand(2222222);

 printf("Network configured and loaded in %f seconds\n", sec(clock()-time));
 chdir(workingDir);
 return yolo;
}

yolo_detection *yolo_detect(yolo_object *yolo, char *filename, float thresh)
{
 if(yolo == NULL)
 {
  return NULL;
 }

 layer l=yolo->net->layers[yolo->net->n-1];
 clock_t time;
 float nms=.4;

 image im=load_image_color(filename, 0, 0);
 image sized=resize_image(im, yolo->net->w, yolo->net->h);
 float *X=sized.data;
 time=clock();
 network_predict(yolo->net, X);
 printf("%s: Predicted in %f seconds.\n", filename, sec(clock()-time));

 int nboxes=0;
 detection *dets=get_network_boxes(yolo->net, 1, 1, thresh, 0, 0, 0, &nboxes);
 if(nms)
 {
  do_nms_sort(dets, l.side*l.side*l.n, l.classes, nms);
 }

 free_image(im);
 free_image(sized);
#ifdef OPENCV
 cvWaitKey(0);
       cvDestroyAllWindows();
#endif
 void *obj=calloc(1, sizeof(yolo_detection));
 if(obj == NULL)
 {
  return NULL;
 }
 yolo_detection *detec=obj;
 detec->detection=dets;
 detec->num_boxes=nboxes;
 return detec;
}

void yolo_detection_free(yolo_detection **yolo)
{
 yolo_detection *detection=(*yolo);
 free_detections(detection->detection, detection->num_boxes);
 free(detection);
 (*yolo)=NULL;
}