import type { ApiMessage } from "../../common/types/api.types";
import type { ScreenInfo, GraphicsConfig } from "./config.types";
import { sendReloadSignal } from "../../common/system/signal.service";
import fs from "node:fs";
import path from "node:path";

const DEFAULT_CONFIG_PATH = path.resolve(__dirname, "../../../../../config/graphics.json");

export async function readConfig(): Promise<GraphicsConfig | null> {
  try {
    const data = await fs.promises.readFile(DEFAULT_CONFIG_PATH, "utf-8");

    if (data.trim().length === 0) {
      return null;
    }

    const currentConfig = JSON.parse(data) as GraphicsConfig;
    return currentConfig;
  } 
  catch (err) {
    if ((err as NodeJS.ErrnoException).code === "ENOENT") {
      return null;
    }

    throw err;
  }
}

export async function getScreenNames(): Promise<string[] | null> {
  const config: GraphicsConfig | null = await readConfig();
  if (config === null || config.screens === null) {
    return null;
  }


  let screenNames: string[] = config.screens!.map((screen) => screen.name);

  return screenNames;
}

export async function getScreenById(screen: string): Promise<ScreenInfo | null> {
  const config: GraphicsConfig | null = await readConfig();
  if (config === null) {
    return null;
  }

  const matchedScreen = config.screens.find((currScreen) => currScreen.name === screen);
  return matchedScreen ?? null;
}

export async function deleteScreenById(screen: string): Promise<ApiMessage> {
  const config: GraphicsConfig | null = await readConfig();
  if (config === null) {
    return {msg: "fail"};
  }

  const screenIdx = config.screens.findIndex((currScreen) => currScreen.name === screen);
  if (screenIdx == -1){
    return {msg: "fail"};
  }
  config.screens.splice(screenIdx, 1);

  await atomicWriteJson(DEFAULT_CONFIG_PATH, config);
  return {msg: "Screen Deleted"};
}

async function atomicWriteJson(filePath: string, value: unknown): Promise<void> {
  const dir = path.dirname(filePath);
  const base = path.basename(filePath);
  const tmpPath = path.join(dir, `.${base}.${process.pid}.${Date.now()}.tmp`);

  const payload = JSON.stringify(value, null, 2);

  try {
    await fs.promises.writeFile(tmpPath, payload, { encoding: "utf-8" });
    await fs.promises.rename(tmpPath, filePath);
  } catch (err) {
    await fs.promises.unlink(tmpPath).catch(() => {});
    throw err;
  }
}


// Writes Graphics Config to shared memory
export async function saveScreen( screenId:string, newScreen: ScreenInfo): Promise<ApiMessage> {
  let config: GraphicsConfig | null = await readConfig();

  if (config === null) {
    config = {screens: [newScreen]}
    await atomicWriteJson(DEFAULT_CONFIG_PATH, config);
    // sendReloadSignal("fsae-graphics.service");
    return {msg: "Config Created"}
  }
  
  let screenUpdated = false; 
  config.screens.forEach( screen => {
    if (screen.name === screenId) {
      screen.name = newScreen.name;
      screen.widgets = newScreen.widgets;
      screenUpdated = true;
    }
  });

  if (screenUpdated === false) {
    config.screens.push(newScreen);
  }

  await atomicWriteJson(DEFAULT_CONFIG_PATH, config);
  // sendReloadSignal("fsae-graphics.service");
  return {msg: "Config Updated"};
}
