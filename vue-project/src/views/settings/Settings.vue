<template>
  <h1>Settings</h1>
  <el-form :model="form" label-width="150" style="width: 2000">
    <el-form-item label="Board">
      <el-select v-model="form.board" placeholder="please select your board">
        <el-option label="T-Beam 0.7" value="tbeam 0.7" />
        <el-option label="T-Beam 1.0" value="tbeam 1.0" />
        <el-option label="T-Beam 1.1" value="tbeam 1.1" />
        <el-option label="T-Beam 1.2" value="tbeam 1.2" />
      </el-select>
    </el-form-item>
    <el-form-item label="Network operator">
      <el-select
        v-model="form.lorawan.server"
        placeholder="Please select your network operator"
      >
        <el-option label="Helium" value="helium" />
        <el-option label="The Things Network" value="ttn" :disabled="true" />
      </el-select>
    </el-form-item>
    <el-form-item label="DevEUI">
      <el-input v-model="form.lorawan.deveui" style="max-width: 350px" />
    </el-form-item>
    <el-form-item label="APPEUI">
      <el-input v-model="form.lorawan.appeui" style="max-width: 350px" />
    </el-form-item>
    <el-form-item label="APPKEY">
      <el-input v-model="form.lorawan.appkey" style="max-width: 350px" />
    </el-form-item>
    <!-- <el-form-item label="Spreading Factor">
    <el-select v-model="form.lorawan.sf" placeholder="Please select spreading factor">
        <el-option label="DR_SF7" :value=3 />
        <el-option label="DR_SF8" :value=2 />
        <el-option label="DR_SF9" :value=1 />
        <el-option label="DR_SF10" :value=0 />
    </el-select>
    <el-popover :width="300">
        <template #reference >
        <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
        <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
        >
            <p class="demo-rich-content__desc" style="margin: 0">
            Spreading Factor (Data Rate) determines how long each 11-byte
            Mapper Uplink is on-air, and how observable it is.
            </p>
            <p class="demo-rich-content__desc" style="margin: 0">
            SF10 is about two seconds per packet, and the highest range,
            while SF7 is a good compromise for moving vehicles and
            reasonable mapping observations.
            </p>
        </div>
        </template>
    </el-popover>
    </el-form-item> -->
    <el-form-item label="ACK request">
      <el-select v-model="form.lorawan.ack" placeholder="">
        <el-option label="never" :value="0" />
        <el-option label="always" :value="1" :disabled="true" />
        <el-option label="every-other-one" :value="2" :disabled="true" />
      </el-select>
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              Confirmed packets (ACK request) conflict with the function of a
              Mapper and should not normally be enabled.
            </p>
            <p>
              In areas of reduced coverage, the Mapper will try to send each
              packet six or more times with different SF/DR. This causes
              irregular results and the location updates are infrequent,
              unpredictable, and out of date.
            </p>
            <p>0 - means never, 1 - means always, 2 - every-other-one</p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-divider />

    <el-form-item label="Minimum Distance">
      <el-input-number v-model="form.mapper.minDistance" :step="1" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              Minimum Distance between Mapper reports. turn the screen off.
            </p>
            <p>
              Minimum distance in meters from the last sent location before we
              send again.
            </p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-form-item label="Stationary Tx Interval">
      <el-input-number v-model="form.mapper.stationaryTxInterval" :step="1" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              If we are not moving at least MIN_DIST meters away from the last
              uplink, when should we send a redundant Mapper Uplink from the
              same location?
            </p>
            <p>
              This Heartbeat or ping isn't all that important for mapping, but
              might be useful for time-at-location tracking or other monitoring.
            </p>
            <p>You can safely set this value very high.</p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-form-item label="Never Rest">
      <el-switch v-model="form.mapper.neverRest" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              Change to 1 if you want to always send at THIS rate, with no
              slowing or sleeping.
            </p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-form-item label="Rest Wait Time">
      <el-input-number v-model="form.mapper.restWaitTime" :step="1" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              If we still haven't moved in this many seconds, start sending even
              slower
            </p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-form-item label="Rest Tx Interval">
      <el-input-number v-model="form.mapper.restTxInterval" :step="1" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              Slow resting ping frequency in seconds
            </p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-form-item label="Sleep Wait Time">
      <el-input-number v-model="form.mapper.sleepWaitTime" :step="1" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              If we STILL haven't moved in this long, turn off the GPS to save
              power
            </p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-form-item label="Sleep Tx Interval">
      <el-input-number v-model="form.mapper.sleepTxInterval" :step="1" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              For a vehicle application where USB Power appears BEFORE motion,
              this can be set very high without missing anything:
            </p>
            <p class="demo-rich-content__desc" style="margin: 0">
              Wake up and check position every now and then to see if movement
              happened.
            </p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-form-item label="GPS Lost Wait Time">
      <el-input-number v-model="form.mapper.lostWaitTime" :step="1" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              How long to wait for a GPS fix before declaring failure
            </p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-form-item label="GPS Lost Ping Time">
      <el-input-number v-model="form.mapper.lostPingTime" :step="1" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              Without GPS reception, how often to send a non-mapper status
              packet
            </p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-divider />

    <el-form-item label="Off Time">
      <el-input-number v-model="form.screen.offTime" :step="1" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              If there are no Uplinks or button presses sent for this long, turn
              the screen off.
            </p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-form-item label="Menu Timeout">
      <el-input-number v-model="form.screen.menuTimeout" :step="1" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              Seconds to wait before exiting the menu.
            </p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-divider />

    <el-form-item label="Deadzone Lat">
      <el-input-number v-model="form.deadzone.lat" :step="0.0001" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              Deadzone defines a circular area where no map packets will
              originate.
            </p>
            <p>
              This is useful to avoid sending many redundant packets in your own
              driveway or office, or just for local privacy.
            </p>
            <p>
              You can "re-center" the deadzone from the screen menu. Set Radius
              to zero to disable altogether.
            </p>
            <p>Thanks to @Woutch for the name</p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-form-item label="Deadzone Lon">
      <el-input-number v-model="form.deadzone.lon" :step="0.0001" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              Deadzone defines a circular area where no map packets will
              originate.
            </p>
            <p>
              This is useful to avoid sending many redundant packets in your own
              driveway or office, or just for local privacy.
            </p>
            <p>
              You can "re-center" the deadzone from the screen menu. Set Radius
              to zero to disable altogether.
            </p>
            <p>Thanks to @Woutch for the name</p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-form-item label="Deadzone Radius">
      <el-input-number v-model="form.deadzone.radius" :step="1" />
      <el-popover :width="400">
        <template #reference>
          <el-icon style="margin-left: 10px"><InfoFilled /></el-icon>
        </template>
        <template #default>
          <div
            class="demo-rich-conent"
            style="display: flex; gap: 16px; flex-direction: column"
          >
            <p class="demo-rich-content__desc" style="margin: 0">
              Deadzone defines a circular area where no map packets will
              originate.
            </p>
            <p>
              This is useful to avoid sending many redundant packets in your own
              driveway or office, or just for local privacy.
            </p>
            <p>
              You can "re-center" the deadzone from the screen menu. Set Radius
              to zero to disable altogether.
            </p>
            <p>Thanks to @Woutch for the name</p>
          </div>
        </template>
      </el-popover>
    </el-form-item>

    <el-form-item>
      <el-button type="primary" @click="onSubmit">Save</el-button>
    </el-form-item>
  </el-form>
</template>

<script>
import { reactive } from "vue";
import axios from "axios";

// do not use same name with ref
const form1 = reactive({
  board: "tbeam 1.1",
  lorawan: {
    server: "helium",
    sf: 3,
    ack: 0,
    deveui: "6081F94FD9D9F631",
    appeui: "6081F93E09B00066",
    appkey: "AE3EEBB241C5CCDEBE0DE89CB139E1D5",
  },
  mapper: {
    minDistance: 70.0,
    stationaryTxInterval: 5 * 60,
    neverRest: false,
    restWaitTime: 20 * 60,
    restTxInterval: 30 * 60,
    sleepWaitTime: 2 * 60 * 60,
    sleepTxInterval: 1 * 60 * 60,
    lostWaitTime: 5 * 60,
    lostPingTime: 15 * 60,
  },
  deadzone: {
    lat: 34.5678,
    lon: -123.4567,
    radius: 500,
  },
  screen: {
    offTime: 2 * 60,
    menuTimeout: 5,
  },
  battery: {
    lowVoltage: 3.1,
  },
});

const API = axios.create({
  //   baseURL: 'http://localhost:3000',
  baseURL: "http://lilygo.local",
  timeout: 1000,
});

export default {
  data() {
    return {
      form: form1,
    };
  },
  mounted() {
    API.get("/api/v1/config")
      .then((response) => (this.form = response.data))
      .catch(function (error) {
        console.log(error);
      });
  },
  methods: {
    onSubmit() {
      console.log("submit: " + this.form);
      API.post("/api/v1/config", this.form, {
        headers: {
          "Content-Type": "application/json",
        },
      })
        .then((res) => {
          console.log(res.data);
        })
        .catch((Error) => {
          console.log(Error);
        });
    },
  },
};
</script>

<style scoped lang="scss"></style>
