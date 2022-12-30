// /src/router/router.ts
// 引入 vue-router
import { createRouter, createWebHistory, RouteRecordRaw } from 'vue-router'
import AppLayout from '@/views/layout/index.vue'

const routes: RouteRecordRaw[] = [
  {
    path: '/',
    name: 'Info',
    redirect: '/info',
    component: AppLayout,
    children: [
      {
        path: '/info',
        name: 'Info',
        component: () => import('@/views/info/Info.vue')
      },
      {
        path: '/settings',
        name: 'Settings',
        component: () => import('@/views/settings/Settings.vue')
      },
      {
        path: '/upgrade',
        name: 'Upgrade',
        component: () => import('@/views/upgrade/Upgrade.vue')
      }
    ]
  },
  {
    path: '/settings',
    component: AppLayout,
    redirect: 'settings',
    children: [
      {
        path: 'settings',
        component: () => import('@/views/settings/Settings.vue')
      }
    ]

  }
]
// createRouter创建路由
const router = createRouter({
  history: createWebHistory(),
  routes,
})
// 最后导出。es6的模块化方式
export default router
