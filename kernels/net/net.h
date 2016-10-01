/**
 *  Customization
 */
typedef float dType;
#define MIN_NUM FLT_MIN
#define MAX_NUM FLT_MAX

//////////////////////////////////////////////////////////////////////
/**
 * Don't change anything below if you don't understand.
 */
#define GLOBAL_SIZE_0 get_global_size(0)
#define GLOBAL_SIZE_1 get_global_size(1)
#define GLOBAL_SIZE_2 get_global_size(2)
#define GLOBAL_SIZE (GLOBAL_SIZE_0 * GLOBAL_SIZE_1 * GLOBAL_SIZE_2)

#define LOCAL_SIZE_0 get_local_size(0)
#define LOCAL_SIZE_1 get_local_size(1)
#define LOCAL_SIZE_2 get_local_size(2)
#define LOCAL_SIZE (LOCAL_SIZE_0 * LOCAL_SIZE_1 * LOCAL_SIZE_2)

#define NUM_GROUP_0 get_num_groups(0)
#define NUM_GROUP_1 get_num_groups(1)
#define NUM_GROUP_2 get_num_groups(2)
#define GROUP_SIZE (GROUP_SIZE_0 * GROUP_SIZE_1 * GROUP_SIZE_2)

#define GLOBAL_ID_0 get_global_id(0)
#define GLOBAL_ID_1 get_global_id(1)
#define GLOBAL_ID_2 get_global_id(2)
#define GLOBAL_ID                                                              \
  ((GLOBAL_ID_0 * GLOBAL_SIZE_1 * GLOBAL_SIZE_2) +                             \
   (GLOBAL_ID_1 * GLOBAL_SIZE_2) + GLOBAL_ID_2)

#define LOCAL_ID_0 get_local_id(0)
#define LOCAL_ID_1 get_local_id(1)
#define LOCAL_ID_2 get_local_id(2)
#define LOCAL_ID                                                               \
  ((LOCAL_ID_0 * LOCAL_SIZE_1 * LOCAL_SIZE_2) + (LOCAL_ID_1 * LOCAL_SIZE_2) +  \
   LOCAL_ID_2)

#define GROUP_ID_0 get_group_id(0) // channel
#define GROUP_ID_1 get_group_id(1) // height
#define GROUP_ID_2 get_group_id(2) // width
#define GROUP_ID                                                               \
  ((GROUP_ID_0 * GROUP_SIZE_1 * GROUP_SIZE_2) + (GROUP_ID_1 * GROUP_SIZE_2) +  \
   GROUP_ID_2)

/**
 * [ELM feature map memory accessment helper]
 * @param  array     [feature map memory, assume to be
 * memory[channel][height][width]]
 * @param  channelId [channel id]
 * @param  nHeight   [height (2nd dimension size)]
 * @param  heightId  [height id]
 * @param  nWidth    [width (1st dimension size)]
 * @param  widthId   [width id]
 * @return           [value dType]
 */
#define ELM(array, channelId, nHeight, heightId, nWidth, widthId)              \
  (array)[(channelId) * (nHeight) * (nWidth) + (heightId) * (nWidth) +         \
          (widthId)]

/**
 * [WELM weight memory accessment helper]
 * @param  weight     [weight memory, assume to be
 * memory[numFilter][channel][height][width]]
 * @param  filterId   [filter id]
 * @param  nChannel   [number of channel (3rd dimension size)]
 * @param  channelId  [channel id]
 * @param  kernelSize [kernel size (2nd,1st dimension size)]
 * @param  heightId   [height id]
 * @param  widthId    [width id]
 * @return            [value dType]
 */
#define WELM(weight, filterId, nChannel, channelId, kernelSize, heightId,      \
             widthId)                                                          \
  (weight)[(filterId) * (kernelSize) * (kernelSize) * (nChannel) +             \
           (channelId) * (kernelSize) * (kernelSize) +                         \
           (heightId) * (kernelSize) + (widthId)]

/**
 * [WORK_ITEM_BEGIN
 * Utility to write simple 1D parrallel code for opencl kernel to process 1d
 * data, code block BEGIN]
 * @param  counter [current global id of this work item to be used in parrallel
 * code block]
 * @param  total   [the total number of data to be processed]
 * @param  id      [starting global id of this work item]
 * @return         [code generated]
 */
#define WORK_ITEM_BEGIN(counter, total, id)                                    \
  __private int counter = (id);                                                \
  while ((counter) < (total)) {

/**
 * [WORK_ITEM_END
 * Utility to write simple 1D parrallel code for opencl kernel to process 1d
 * data, code block END
 * **everything in between will execute per work item by <counter>**]
 * @param  counter   [current global id of this work item]
 * @param  groupSize [global size of all work items]
 * @return           [code generated]
 */
#define WORK_ITEM_END(counter, groupSize)                                      \
  (counter) += (groupSize);                                                    \
  }

/**
 * [WORK_ITEM_3D_BEGIN
 * Utility to write simple 3D parrallel code for opencl kernel to process 3d
 * feature map, BEGIN
 * Parallel code block start, **everything in between will execute per work item
 * by <channelCounter, heightCounter, widthCounter>**]
 * @param  channelCounter [current work item channel id that will be used in
 * code block inside]
 * @param  numChannel     [how many channel we have, usually
 * param.outputChannel]
 * @param  channelId      [start global id on channel, usually GLOBAL_SIZE_0]
 * @param  heightCounter  [current work item height id that will be used in code
 * block inside]
 * @param  numHeight      [feature map height, usually param.outputHeight]
 * @param  heightId       [start global id on height, usually GLOBAL_SIZE_1]
 * @param  widthCounter   [current work item width id that will be used in code
 * block inside]
 * @param  numWidth       [feature map width, usually param.outputWidth]
 * @param  widthId        [start global id on width, usually GLOBAL_SIZE_2]
 * @return                [code generated]
 */
#define WORK_ITEM_3D_BEGIN(channelCounter, numChannel, channelId,              \
                           heightCounter, numHeight, heightId, widthCounter,   \
                           numWidth, widthId)                                  \
  __private int channelCounter = (channelId), heightCounter, widthCounter;     \
  while (channelCounter < (numChannel)) {                                      \
    heightCounter = (heightId);                                                \
    while (heightCounter < (numHeight)) {                                      \
      widthCounter = (widthId);                                                \
      while (widthCounter < (numWidth)) {

/**
 * [WORK_ITEM_3D_END
 * Utility to write simple 3D parrallel code for opencl kernel to process 3d
 * feature map, END
 * Parallel code block end, **everything in between will execute per work item
 * by <channelCounter, heightCounter, widthCounter>**]
 * @param  channelCounter   [current work item channel id that will be used in
 * code block inside]
 * @param  channelGroupSize [global size of work items on channel dimension]
 * @param  heightCounter    [current work item height id that will be used in
 * code block inside]
 * @param  heightGroupSize  [global size of work items on height dimension]
 * @param  widthCounter     [current work item width id that will be used in
 * code block inside]
 * @param  widthGroupSize   [global size of work items on width dimension]
 * @return                  [code generated]
 */
#define WORK_ITEM_3D_END(channelCounter, channelGroupSize, heightCounter,      \
                         heightGroupSize, widthCounter, widthGroupSize)        \
  widthCounter += (widthGroupSize);                                            \
  }                                                                            \
  heightCounter += (heightGroupSize);                                          \
  }                                                                            \
  channelCounter += (channelGroupSize);                                        \
  }

/**
 * [LOAD_DATA load data from src to dest]
 * @param  src       [memory source]
 * @param  dest      [memory destination]
 * @param  total     [total number of data will be transfered, usually
 * inputTotalDataNum]
 * @param  id        [description the gloabl id of current work item]
 * @param  groupSize [description the global size of this opencl kernel]
 */
#define LOAD_DATA(src, dest, total, id, groupSize)                             \
  WORK_ITEM_BEGIN(loadDataCounter, total, id)                                  \
  (dest)[loadDataCounter] = (src)[loadDataCounter];                            \
  WORK_ITEM_END(loadDataCounter, groupSize)

/**
 * [LOAD_DATA_SCALE load data from src to dest, with a scale parameter]
 * @param  src       [memory source]
 * @param  dest      [memory destination]
 * @param  total     [total number of data will be transfered, usually
 * inputTotalDataNum]
 * @param  id        [description the gloabl id of current work item]
 * @param  groupSize [description the global size of this opencl kernel]
 * @param  scale     [the scaling factor]
 * @return           [code generated]
 */
#define LOAD_DATA_SCALE(src, dest, total, id, groupSize, scale)                \
  WORK_ITEM_BEGIN(loadDataCounterScale, total, id)                             \
  (dest)[loadDataCounterScale] = (src)[loadDataCounterScale] * (scale);        \
  WORK_ITEM_END(loadDataCounterScale, groupSize)

/**
 * [LOAD_DATA_PAD description]
 * @param  src              [memory source]
 * @param  dest             [memory destination]
 * @param  numChannel       [how many channel of the feature map]
 * @param  channelGroupSize [the global size on the channel dimension]
 * @param  channelId        [the global id on the channel dimension]
 * @param  numHeight        [the height of each feature map]
 * @param  heightGroupSize  [global size on height]
 * @param  heightId         [global id on height]
 * @param  numWidth         [the width of each feature map]
 * @param  widthGroupSize   [global size on width]
 * @param  widthId          [global id on width]
 * @param  pad              [padding parameter]
 * @return                  [code generated]
 */
#define LOAD_DATA_PAD(src, dest, numChannel, channelGroupSize, channelId,      \
                      numHeight, heightGroupSize, heightId, numWidth,          \
                      widthGroupSize, widthId, pad)                            \
  WORK_ITEM_3D_BEGIN(channelCounter, numChannel, channelId, heightCounter,     \
                     numHeight, heightId, widthCounter, numWidth, widthId)     \
  ELM(dest, channelCounter, numHeight + 2 * (pad), heightCounter + (pad),      \
      numWidth + 2 * (pad), widthCounter + (pad)) =                            \
      ELM(src, channelCounter, numHeight, heightCounter, numWidth,             \
          widthCounter);                                                       \
  WORK_ITEM_3D_END(channelCounter, channelGroupSize, heightCounter,            \
                   heightGroupSize, widthCounter, widthGroupSize)

/**
 * [RELU(x) relu activation]
 * @param  x [input value]
 * @return   [x if x > 0, else 0]
 */
#define RELU(x) ((x) > 0 ? (x) : 0)

/**
 * Network Parameter structure, pass parameter from cpp to fpga easily with
 * this.
 */
typedef struct {
  dType scale;
  int stride;
  int kernelSize;
  int pad;
  int dilation;
  int inputChannel;
  int inputHeight;
  int inputWidth;
  int outputChannel;
  int outputHeight;
  int outputWidth;
  int inputTotalDataNum;
  int outputTotalDataNum; // padding layer also has this!
} NetParam;
