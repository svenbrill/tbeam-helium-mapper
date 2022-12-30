<template>
  <div class="logo">

  </div>
  <el-menu
    default-active="2"
    :collapse="props.isCollapse"
    :collapse-transition="false"
    @open="handleOpen"
    @close="handleClose"
    background-color="#ebf1f5"
    text-color="#606266"
    active-text-color="#2F74FF"
    class="menu"
    router
  >
    <el-menu-item index="/info">
      <el-icon><icon-menu /></el-icon>
      <template #title>Info</template>
    </el-menu-item>
    <el-menu-item index="/settings">
      <el-icon><setting /></el-icon>
      <template #title>Settings</template>
    </el-menu-item>
    <el-menu-item index="/upgrade">
      <el-icon><Refresh /></el-icon>
      <template #title>Upgrade</template>
    </el-menu-item>
  </el-menu>
</template>

<script lang="ts" setup>
import { ref } from 'vue'
import {
  Document,
  Menu as IconMenu,
  Location,
  Setting,
} from '@element-plus/icons-vue'

const props = defineProps(['isCollapse'])
const handleOpen = (key: string, keyPath: string[]) => {
  console.log(key, keyPath)
}
const handleClose = (key: string, keyPath: string[]) => {
  console.log(key, keyPath)
}
</script>

<style>
.logo{
  height:80px;
  background: #e1eaf4;
}
.menu{
  min-height: calc(100vh - 80px);
}
</style>
