<h1> CFS </h1>

* 問題:

  * 假設 Task A Time Slice 長度為10 ms
  * 以總 Tasks 數量與總 Time Slice 長度來說,會取得處理器5%的執行時間,
  * 最理想的狀況是,我們在任意一秒的間隔來看, Task A 都可以分到處理器5%的執行時間
  * 但在現實的狀況來說,在任意一秒的間隔來看
  * 只要有優先級高的 Task 被排程到且該Task分配到的Time Slice較長,就會讓 Task A 呈現在一個不公平的時間分配中

* 精神:

  * 當處理器有100%能力，且系統中有兩個 tasks 正在運作，這兩個 tasks 可以各自取得處利器 50% 的執行能力，並被處理器平行執行。
  * 也就是說任意時間間格來看(例如，隨機取 10 ms 間隔) 這兩個 task 都是被分配處利器 50% 的執行能力。

* 方法:

  * 導入了”Virtual Runtime”的概念(單位為 nanosec)
  * 根據目前總共的Tasks數量,用以作為選擇下一個執行Task Time Slice的依據
  * 例如:
    * CFS會選擇目前p->se.vruntime值最低的Task來執行 (也就是距離上一次被執行時間間隔最久的Task)

* 每次 Scheduler Tick 觸發時，會進行的動作:

  * 系統的排程都是基於固定的Scheduling Tick來觸發
  * 因此,每一次的Scheduling Tick觸發,都攸關Task執行的週期與是否要進行Task的切換
    * 根據編譯核心時配置的HZ值來決定,一般來說是每1/100秒或每1/1000秒觸發一次Scheduling Tick,讓系統的排程機制可以去計算每個Task執行的時間,並藉此決定是不是要進行Task Context-Switch
    * 關鍵函式/變數: task_tick 

* 如何挑選 task:

  * RB-Tree
  * Nice Priority低的Task Virutal RunTime會增長的比較快,相對佔據處理器執行的時間就會比較短
  * vruntime:
    * 透過 calc_delta_mine 
    * 根據目前Task的優先級計算在排程時每次一個Scheduling Tick要增加的Delta值
    * 也就是說如果該Task Nice值越高 該Delta值就越大
    * 對該執行的Task來說就是Virtual RunTime在運作時,增加的速度越快(等於縮短實際執行的時間)

* 函式/變數:

  * **struct <font color = "red">load_weight load</font>**

    * struct load_weight {

      unsigned long weight, inv_weight;

      };

      

  * **SCHED_LOAD_SCALE**

    *  Nice 0 的 Load Weight=1024

    

  * **\#define likely(x) __builtin_expect(!!(x), 1)**

    * 也就是說明x==1是「經常發生的」或是「很可能發生的」。

  * **static inline unsigned long <font color = "red">calc_delta_fair</font>(unsigned long delta, struct sched_entity \*se)**

    * 當目前的Scheduling Entity Load Weight不為NICE 0 的Load Weight (=1024)時,就會根據所帶入的Delta值與對應的Load Weight,計算出新的Delta值
    * 更新目前運作的Current Task執行的Virtual RunTime時,就會呼叫calc_delta_fair用以取得Delta時間,並累加至變數curr->sum_exec_runtime中
    * 用在判斷是否要中斷目前的Task讓下一個Task (Scheduling Entity)可以進來執行
    * 在取得要置入Run Queue的Task Scheduling Entity的Virtual RunTime Slice值

    

  * **static u64 <font color = "red">sched_slice</font>(struct cfs_rq \*cfs_rq, struct sched_entity \*se)**

    * 取得Task Scheduling Entity的Wall-Time slice.

    

  * **NICE_0_LOAD **

    * 1024

    

  * **WMULT_CONST**

    * 2^32

    

  * **WMULT_SHIFT**

    * 32

    

  * **__update_inv_weight(struct load_weight *lw)**

    * 如果lw->inv_weight不为0，不作任何修改函数直接返回；
    * 否则，更新lw->inv_weight的值为lw->inv_weight = max(1, min(WMULT_CONST * 1024 / lw->weight, WMULT_CONST))
    * lw->inv_weight的最大值就是WMULT_CONST, 最小值为1, 也可以表述为在2^32取值内

    

  * **static unsigned long <font color = "red"> calc_delta_mine</font>(unsigned long delta_exec, unsigned long weight, struct load_weight *lw)**

    * delta_exec

      * 是目前Virtual RunTime Slice的Delta值

    * weight

      * 為目前Task的Load Weight,
      * 若Task的Nice值為 0,則此值為1024
      * 這個**值越大**,  inv_weight 值越小,
      * 對應該 Task所計算出來的**Delta值也越小**
      * **Virtual RunTime增加的越慢**,因此實際得到處理器執行的真實時間就越長

    * lw->weight

      * 為目前排程單位sched_entity的Load Weight

    * 處理步驟: 

      1. 如果lw->inv_weight 為0

         lw->inv_weight = 1 + (WMULT_CONST-lw->weight/2) / (lw->weight+1);

      2. Temp-Delta值為delta_exec * weight
         1. 如果Temp-Delta >  2^32

            Delta = SRR(SRR(Temp-Delta, WMULT_SHIFT/2) * lw->inv_weight,WMULT_SHIFT/2);

         2. 如果Temp-Delta <=  2^32

            Delta = SRR(Temp-Delta * lw->inv_weight, WMULT_SHIFT);

  

* 如果Nice值加一 ,大約會減少10%處理器的執行時間而如果是Nice值減一(也就是優先級調升),大約就會增加10%處理器的執行時間

  * 每個Nice值的區間所對應的Weight大約會相差1.25倍



帶整理: https://www.itread01.com/content/1541862331.html