import type { Request, Response } from "express";
import * as configService from "./config.service";

export async function getConfig(_req: Request, res: Response) {
  const config = await configService.readConfig();
  res.status(200).json(config);
}

export async function updateConfig(req: Request, res: Response) {
  const updated = await configService.writeConfig(req.body);
  res.status(200).json(updated);
}

export async function resetConfig(_req: Request, res: Response) {
  const defaults = await configService.resetConfig();
  res.status(200).json(defaults);
}
