# git的基本用法

## git的分支
当对git的分支功能有了解后,我们往往会有这样的疑惑,我们应该怎么样使用分支,包括什么时候需要一个新的分支,分支之间应该怎么保证一致性从而减少merge时候的问题,多人协作时应该怎么组织分支来使得项目更有条理.
### 什么时候需要一个新的分支

### 分支之间应该怎么保证一致性
在项目刚开始的时候搭建易用框架,在框架下编程可以很大程度上控制merge时的冲突

### 怎么组织分支来使得项目更有条理
当前我们是一个开发者一个分支,周期地将代码merge至develop分支,当版本稳定后merge至master分支.

### 分支常用命令
```
git merge <分支>   //将这个分支合并到当前分支
git branch        //查看当前分支
git branch -d <分支>    //删除这个分支
git checkout <分支>     //切换到这个分支
git checkout -b <分支>  //创建并切换至这个分支
git fetch origin <分支> //从远程仓库拉取分支
git pull                //git fetch + git merge
git diff <分支>          //比较当前分支和该分支的不同
```
```
$ git fetch origin master           //从远程的origin仓库的master分支下载代码到本地的origin master
$ git log -p master.. origin/master //比较本地的仓库和远程参考的区别
$ git merge origin/master           //把远程下载下来的代码合并到本地仓库，远程的和本地的合并
```
