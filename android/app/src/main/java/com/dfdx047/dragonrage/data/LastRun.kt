package com.dfdx047.dragonrage.data

import java.io.File

/**
 * How the game's last run ended, from its log (bt3_log.txt): the engine and its loader write why they stop. Read when
 * the launcher comes back; each log is looked at once (its time is kept in .lastrun_seen), so a problem is told once.
 */
object LastRun {
    enum class Problem { DATA, MEMORY, ENGINE, CRASH }

    data class Report(val problem: Problem, val detail: String)

    fun check(paths: GamePaths): Report? {
        val log = File(paths.root, "bt3_log.txt")
        val seen = File(paths.root, ".lastrun_seen")
        if (!log.exists()) return null
        val stamp = log.lastModified().toString()
        if (runCatching { seen.readText() }.getOrNull() == stamp) return null
        runCatching { seen.writeText(stamp) }
        val lines = runCatching { log.readLines() }.getOrNull() ?: return null
        fun find(vararg keys: String) = lines.lastOrNull { l -> keys.any { l.contains(it) } }
        find("CRASH: signal")?.let { return Report(Problem.CRASH, it.substringAfter("CRASH: ").trim()) }
        find("The game's data comes from your own disc image", "not the unmodified USA release", "damaged file", "was not found")?.let {
            val i = lines.indexOf(it)
            val reason = (if (it.contains("comes from your own disc image") && i > 0) lines[i - 1] else it).substringAfter("bt3: ").trim()
            return Report(Problem.DATA, reason)
        }
        find("no room below 4 GB", "no free place below 4 GB", "no memory below 4 GB")?.let { return Report(Problem.MEMORY, it.substringAfter("bt3: ").trim()) }
        find("the engine did not load", "cannot read the engine", "the engine has no entry point")?.let { return Report(Problem.ENGINE, it.trim()) }
        return null
    }
}
