# Seize and Secure - Phoenix

**Arma Reforger Custom Scenario and Gameplay Adjustments Mod**

Welcome to the **Seize and Secure - Phoenix** repository! This project revives the original "Seize and Secure" mod, crafted by our friend Ironbeard—who poured his heart and spare time into it before stepping away. Like a phoenix rising from the ashes, this community effort is a big thank you to him, keeping the spirit alive through collaboration and updates.

Fork, contribute, or join in to enhance the Arma Reforger experience. Check the [Arma Reforger Modding Wiki](https://community.bistudio.com/wiki/Category:Arma_Reforger) for game-specific integration.

## Table of Contents
- [Description](#description)
- [Requirements](#requirements)
- [Setup Instructions](#setup-instructions)
- [Cloning the Repository](#cloning-the-repository)
- [Working with Files](#working-with-files)
- [Contributing](#contributing)
- [Troubleshooting](#troubleshooting)
- [License](#license)

## Description
**Seize and Secure - Phoenix** delivers dynamic custom scenarios and refined gameplay tweaks for Arma Reforger. Building on Ironbeard's foundational work, this version unites collaborators to evolve and maintain the mod—honoring his dedication while pushing forward.

## Requirements
- **Git**: [Download Git](https://git-scm.com/downloads)
- **TortoiseGit**: Windows Git client (recommended). [Download TortoiseGit](https://tortoisegit.org/download/)
- **Git LFS**: For large files. [Download Git LFS](https://git-lfs.com)

**Note**: TortoiseGit is Windows-only. Install Git first—it's a prerequisite.

## Setup Instructions
1. **Install Git**:
   - Download from [git-scm.com/downloads](https://git-scm.com/downloads).
   - Run installer (accept defaults).
   - Verify: Open Command Prompt, run `git --version`.

2. **Install TortoiseGit**:
   - Download latest 64-bit from [tortoisegit.org/download](https://tortoisegit.org/download/).
   - Run `.msi` installer—it auto-detects Git.
   - Restart Explorer (or reboot) for right-click menus.

3. **Install Git LFS**:
   - Download from [git-lfs.com](https://git-lfs.com).
   - Run installer.
   - Open Command Prompt/Git Bash: `git lfs install` (global init).

See [GitHub's Git LFS Guide](https://docs.github.com/en/repositories/working-with-files/managing-large-files/about-git-large-file-storage).

## Cloning the Repository
1. Right-click an empty folder in File Explorer.
2. **TortoiseGit > Clone...**
3. Fill dialog:
   - **URL**: `https://github.com/Megagoth1702/Seize-And-Secure-Phoenix.git`
   - **Directory**: Choose clone location
   - **Check 'LFS'**: Enables Git LFS to download large files (requires Git LFS installed)
4. Click **OK**.
5. Right-click cloned folder > **TortoiseGit > Pull** for latest changes.

## Working with Files
Everything works like standard Git:

1. Add new/changed files: Right-click > **TortoiseGit > Add**.
2. Edit files in your tools.
3. Stage & commit: Right-click folder > **Git Commit...** (select files, add message, **Commit**).
4. Push: Right-click > **Git Push**.

**Git LFS is automatic** for tracked files (via `.gitattributes`).

**Optional: File Locking** (prevents conflicts):
- Right-click file > **TortoiseGit > LFS > Lock**.
- Manage: **TortoiseGit > LFS > Locks...**.

## Contributing
1. **Collaborators**: Create branch (**TortoiseGit > Create Branch...**), commit/push, create PR on GitHub.
2. **Forks**: Fork on GitHub, clone your fork, PR to upstream.
3. Follow [GitHub Flow](https://docs.github.com/en/get-started/quickstart/github-flow).

Add a `CONTRIBUTING.md` for detailed guidelines.

## Troubleshooting
| Issue | Solution |
|-------|----------|
| Large files are tiny (~1KB text)? | Git LFS missing: Reinstall + `git lfs install`, then in repo: `git lfs pull` or reclone with LFS enabled. |
| Push fails ("over quota")? | Free accounts: 10 GiB storage + 10 GiB monthly bandwidth. Check [GitHub Billing > Git LFS](https://github.com/settings/billing). To clean old large files from history, use [BFG Repo-Cleaner](https://rtyley.github.io/bfg-repo-cleaner/)—see this [beginner's guide](https://grok.com/share/c2hhcmQtMi1jb3B5_a13ed4b8-e1b4-46e8-8280-9f903f84c63c) for steps. |
| No 'LFS' checkbox? | Install Git LFS first (version 2.10+ TortoiseGit). |
| Slow clone? | Large repo—normal with LFS. |

## License
This project is licensed under the Arma Public License Share Alike (APL-SA) - see the full license at [https://www.bohemia.net/community/licenses/arma-public-license-share-alike](https://www.bohemia.net/community/licenses/arma-public-license-share-alike).