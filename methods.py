import os
import configparser


def generate_version_files() -> None:
    plugin_cfg = configparser.ConfigParser()
    plugin_cfg.read('addons/gdn-sqlite/plugin.cfg')

    plugin_cfg_gen = open("include/gdn/sqlite/plugin_cfg.gen.h", "w")
    plugin_cfg_gen.write(
        f'#define PLUGIN_NAME {plugin_cfg["plugin"]["name"]}\n')
    plugin_cfg_gen.write(
        f'#define PLUGIN_DESCRIPTION {plugin_cfg["plugin"]["description"]}\n')
    plugin_cfg_gen.write(
        f'#define PLUGIN_AUTHOR {plugin_cfg["plugin"]["author"]}\n')
    plugin_cfg_gen.write(
        f'#define PLUGIN_VERSION {plugin_cfg["plugin"]["version"]}\n')
    plugin_cfg_gen.close()

    githash = ""
    gitfolder = ".git"

    if os.path.isfile(".git"):
        module_folder = open(".git", "r").readline().strip()
        if module_folder.startswith("gitdir: "):
            gitfolder = module_folder[8:]

    if os.path.isfile(os.path.join(gitfolder, "HEAD")):
        head = open(os.path.join(gitfolder, "HEAD"), "r").readline().strip()
        if head.startswith("ref: "):
            head = os.path.join(gitfolder, head[5:])
            if os.path.isfile(head):
                githash = open(head, "r").readline().strip()
        else:
            githash = head

    version_gen = open("include/gdn/sqlite/version.gen.h", "w")
    version_gen.write(f'#define GIT_VERSION_HASH "{githash}"\n')
    version_gen.close()
