<template>
  <el-row :gutter="20">
    <el-col :span="6">
      <el-card class="box-card">
        <template #header>
          <div class="card-header">
            <span>System</span>
          </div>
        </template>
        <div class="text item flex">
          <div class='grid5'>Model</div>
          <div class='grid5 text-right'>{{data?.sys?.model}}</div>
        </div>
        <div class="text item flex">
          <div class='grid5'>Architecture</div>
          <div class='grid5 text-right'>{{data?.sys?.arch.mfr + ' ' +
                                          data?.sys?.arch.model + ' ' + 
                                          "rev " + data?.sys?.arch.revision + ' ' +
                                          data?.sys?.arch.cpu + ' @ ' +
                                          data?.sys?.arch.freq + 'Mhz'}}</div>
        </div>
        <div class="text item flex">
          <div class='grid5'>Firmware version</div>
          <div class='grid5 text-right'>{{data?.sys?.fw}}</div>
        </div>
        <div class="text item flex">
          <div class='grid5'>SDK version</div>
          <div class='grid5 text-right'>{{data?.sys?.sdk}}</div>
        </div>
      </el-card>
    </el-col>
    <el-col :span="6">
      <el-card class="box-card">
        <template #header>
          <div class="card-header">
            <span>Memory</span>
          </div>
        </template>
        <div class="text item flex">
          <div class='grid5'>Total Availabl</div>
          <div class='grid5 text-right'>{{(data?.mem?.total / 1024).toFixed(2) + " KB"}}</div>
        </div>
        <div class="text item flex">
          <div class='grid5'>Free</div>
          <div class='grid5 text-right'>{{(data?.mem?.free / 1024).toFixed(2) + " KB"}}</div>
        </div>
      </el-card>
    </el-col>
    <el-col :span="6">
      <el-card class="box-card">
        <template #header>
          <div class="card-header">
            <span>File System </span>
          </div>
        </template>
        <div class="text item flex">
          <div class='grid5'>Total Availabl</div>
          <div class='grid5 text-right'>{{(data?.fs?.total / 1024 / 1024).toFixed(2) + " MB"}}</div>
        </div>
        <div class="text item flex">
          <div class='grid5'>Used</div>
          <div class='grid5 text-right'>{{(data?.fs?.used / 1024 /1024).toFixed(2) + " MB"}}</div>
        </div>
        <div class="text item flex">
          <div class='grid5'>Free</div>
          <div class='grid5 text-right'>{{(data?.fs?.free / 1024 / 1024).toFixed(2) + " MB"}}</div>
        </div>
      </el-card>
    </el-col>
  </el-row>
  <el-row :gutter="20">
    <el-col :span="6">
      <el-card class="box-card">
        <template #header>
          <div class="card-header">
            <span>LoRa</span>
          </div>
        </template>
        <div class="text item flex">
          <div class='grid5'>Band</div>
          <div class='grid5 text-right'>{{data?.lora?.band}}</div>
        </div>
        <div class="text item flex">
          <div class='grid5'>Radio</div>
          <div class='grid5 text-right'>{{data?.lora?.radio}}</div>
        </div>
      </el-card>
    </el-col>
  </el-row>
</template>

<script>
import axios from 'axios'

const API = axios.create({
  // baseURL: 'http://localhost:3000',
  baseURL: 'http://lilygo.local',
  timeout: 1000
})

export default {
  data() {
    return {
      data: null
    }
  },
  mounted() {
    API
      .get('/api/v1/status')
      .then(response => (this.data = response.data))
      .catch(function (error) {
        console.log(error);
      });
  }
}
</script>

<style scoped lang="scss">
.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.el-row {
  margin-bottom: 20px;
}

.text {
  font-size: 14px;
}

.item {
  margin-bottom: 18px;
}

.box-card {
  // width: 480px;
}

.list-h{
  padding:15px 0;
}

.flex{
  display: flex;
}

.grid3 {
  width: 30%;
}

.grid5 {
  width: 50%;
}

.grid7 {
  width: 70%;
}

.text-right {
  text-align: right;
}
</style>
